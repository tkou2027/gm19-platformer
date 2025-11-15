#include "render_system.h"

#include "platform/graphics_gl.h"
#include "global_context.h"
#include "platform/timer.h"
#include "render/texture.h"
#include "render/shaders.h"
#include "scene/scene.h"
#include "component/render_sprite.h"
#include "platform/assert.h"

// ’¸“_î•ñ
namespace foo
{
	void RenderSystem::Initialize()
	{
		// OpenGL Settings ========
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// depth test is disabled
		// as we will sort all layers form bottom to top while rendering
		glDisable(GL_DEPTH_TEST);
		// ƒJƒŠƒ“ƒO‚ðON
		glEnable(GL_CULL_FACE);

		// Camera ========
		m_camera.Initialize(Vector2{ (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT });

		// Frame Buffer ========
		m_frame_buffer.Initialize(SCREEN_WIDTH, SCREEN_HEIGHT);
		m_frame_buffer_trail.Initialize(SCREEN_WIDTH, SCREEN_HEIGHT);
		m_frame_buffer_reflection.Initialize(SCREEN_WIDTH, SCREEN_HEIGHT);
		m_frame_buffer_screen.Initialize(SCREEN_WIDTH, SCREEN_HEIGHT);

		// Data ====
		m_batch_data.Initialize();
		m_space_data.Initialize();
		m_tile_map_data.Initialize();
		m_trail_data.Initialize();
		m_particle_data.Initialize();

		// Vertex Buffer ========
		glGenVertexArrays(1, &m_default_vao);
	}

	void RenderSystem::Finalize()
	{
		glDeleteVertexArrays(1, &m_default_vao);

		m_particle_data.Finalize();
		m_trail_data.Finalize();
		m_tile_map_data.Finalize();
		m_space_data.Finalize();
		m_batch_data.Finalize();

		m_frame_buffer_reflection.Finalize();
		m_frame_buffer_screen.Finalize();
		m_frame_buffer_trail.Finalize();
		m_frame_buffer.Finalize();
	}

	void RenderSystem::ResetCamera()
	{
		m_camera.Initialize(Vector2{ (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT });
	}

	void RenderSystem::ClearBuffers()
	{
		// clear frame buffers
		m_frame_buffer.Clear(Vector4{ 1.0f, 0.5f, 0.5f, 1.0f }); // R
		m_frame_buffer_trail.Clear(Vector4{ 0.0f, 0.0f, 0.0f, 0.0f });
		m_frame_buffer_reflection.Clear(Vector4{ 0.5f, 0.5f, 1.0f, 1.0f }); // G
		m_frame_buffer_screen.Clear(Vector4{ 0.5f, 0.5f, 1.0f, 1.0f }); // B
		// clear display
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	void RenderSystem::Draw()
	{
		UpdateDrawData();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		DrawPassTrail();

		glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer.GetFbo());
		DrawPassDefault();

		glDisable(GL_BLEND);
		DrawPassPostProcess();

		m_camera.Update(); // update pending camera move
	}

	void RenderSystem::UpdateDrawData()
	{
		Scene* scene = g_global_context.m_scene_manager->GetCurrentScene();
		if (!scene)
		{
			return;
		}
		m_batch_data.Clear();
		m_tile_map_data.Clear();
		m_trail_data.Clear();
		m_space_data.Clear();
		m_particle_data.Clear();
		List& objects = scene->GetGameObjects().GetObjectList();
		for (int i = 0; i < objects.GetCount(); i++)
		{
			GameObject* obj = (GameObject*)objects[i];
			if (!obj->GetActive())
			{
				continue;
			}
			RenderComponent* renderer = obj->GetRenderComponent();
			if (!renderer || !renderer->GetActive())
			{
				continue;
			}
			renderer->UpdateRenderData();
		}
		m_batch_data.UpdateDrawData();
	}

	void RenderSystem::DrawPassTrail()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer_trail.GetFbo());
		m_trail_data.Draw();
		m_batch_data.DrawMask();
		m_particle_data.DrawMask();
		m_tile_map_data.DrawMask();
	}

	void RenderSystem::DrawPassDefault()
	{
		ShaderManager* shader_manager = g_global_context.m_shader_manager;
		shader_manager->SetShader(ShaderManager::ShaderName::SPACE);
		shader_manager->SetTexture("uSamplerSpace", m_frame_buffer_trail.GetColorTexture(), 1);

		m_space_data.Draw();

		m_particle_data.Draw();

		g_global_context.m_shader_manager->SetShader(ShaderManager::ShaderName::DEFAULT);
		m_batch_data.Draw();
		m_tile_map_data.Draw();
		m_batch_data.DrawUI();
	}

	// TODO: not used
	void RenderSystem::DrawPassCombineReflection()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer_screen.GetFbo());

		ShaderManager* shader_manager = g_global_context.m_shader_manager;
		shader_manager->SetShader(ShaderManager::ShaderName::COMBINE_REFLECTION);

		// sample default and reflection frame buffer ====
		unsigned int color_texture_default = m_frame_buffer.GetColorTexture();
		unsigned int color_texture_reflection = m_frame_buffer_reflection.GetColorTexture();
		shader_manager->SetTexture("uSamplerDefault", color_texture_default, 0);
		shader_manager->SetTexture("uSamplerReflection", color_texture_reflection, 1);

		// set reflection plane ====
		// TODO
		m_reflection_config.center.y = 0.0f;
		shader_manager->SetVec2("uReflectNormal", m_reflection_config.normal);
		shader_manager->SetVec2("uReflectCenter", m_reflection_config.center);

		// set time for animation ====
		float time = g_global_context.m_timer->GetTime();
		shader_manager->SetFloat("uTime", time);

		// draw call ====
		glBindVertexArray(m_default_vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	void RenderSystem::DrawPassPostProcess()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		ShaderManager* shader_manager = g_global_context.m_shader_manager;
		shader_manager->SetShader(ShaderManager::ShaderName::POST_PROCESS);

		unsigned int color_texture = m_frame_buffer.GetColorTexture();
		shader_manager->SetTexture("uSampler", color_texture);

		glBindVertexArray(m_default_vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	// helper for loading textures ====
	void RenderSystem::LoadSpriteConfig(SpriteConfigInput& input) const
	{
		// load texture
		if (input.texture_path.length() == 0)
		{
			input.texture_id = 0;// g_global_context.m_texture->LoadTexture("white.png");
		}
		else
		{
			input.texture_id = g_global_context.m_texture->LoadTexture(input.texture_path);
		}
		// split tile sheet
		input.uv_size_per_frame.x = input.uv_size.x / input.frame_cols;
		input.uv_size_per_frame.y = input.uv_size.y / input.frame_rows;
	}
}