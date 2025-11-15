#include "render_data_trail.h"

#include "platform/graphics_gl.h"
#include "global_context.h"
#include "render_system.h"
#include "render/shaders.h"
#include "platform/assert.h"

namespace foo
{
	void RenderDataTrail::Initialize()
	{
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);
	}

	void RenderDataTrail::Finalize()
	{
		glDeleteVertexArrays(1, &m_vao);
	}

	void RenderDataTrail::Clear()
	{
		m_trail_cnt = 0;
	}

	void RenderDataTrail::Add(const Trail& trail)
	{
		FOO_ASSERT(m_trail_cnt < MAX_TRAILS - 1, "Trail data overflow");

		m_trails[m_trail_cnt] = Trail{ trail };
		m_trail_cnt++;
	}

	void RenderDataTrail::Draw()
	{
		glBindVertexArray(m_vao);
		// draw effect
		ShaderManager* shader_manager = g_global_context.m_shader_manager;
		shader_manager->SetShader(ShaderManager::ShaderName::TRAIL);

		Vector2 camera_offset = g_global_context.m_render_system->GetCamera().GetOffset();
		shader_manager->SetVec2("uCameraOffset", camera_offset);
		for (int i = 0; i < m_trail_cnt; i++)
		{
			// TODO: type
			const SpaceProp space_prop = g_space_config.GetSpaceProp(m_trails[i].space_id);
			//shader_manager->SetVec4("uColor", space_prop.color);
			shader_manager->SetFloat("uColorMask", space_prop.color_mask);
			const TrailShape& trail_shape = m_trails[i].shape;
			shader_manager->SetFloat("uShapeType", trail_shape.GetTypeFloat());
			shader_manager->SetVec2("uCenter", trail_shape.center);
			shader_manager->SetVec2("uNormal", trail_shape.normal);
			shader_manager->SetFloat("uRadius", trail_shape.radius);

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
	}
}

