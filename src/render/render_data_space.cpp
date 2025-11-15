#include "render_data_space.h"

#include "platform/graphics_gl.h"
#include "platform/assert.h"
#include "global_context.h"
#include "render_system.h"
#include "shaders.h"
#include "texture.h"
#include "render_data.h"
#include "resource/render_layer.h"

namespace foo
{
	void RenderDataSpace::Initialize()
	{
		m_batch_data_list.Initialize(MAX_BATCHES);

		// buffer
		// create vao ====
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		// instance
		glGenBuffers(1, &m_instance_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_instance_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(DataInstance) * MAX_INSTANCES_PER_BRANCH, 0, GL_DYNAMIC_DRAW);
		glVertexArrayVertexBuffer(m_vao, 1, m_instance_vbo, 0, sizeof(DataInstance));
		// instance attribute ====
		unsigned int offset{ 0 };
		// 0. offset
		offset = RenderData::InitializeInstanceAttrib(m_vao, 0, 2, offset);
		// 1. size
		offset = RenderData::InitializeInstanceAttrib(m_vao, 1, 2, offset);
		// 2. uv_size
		offset = RenderData::InitializeInstanceAttrib(m_vao, 2, 2, offset);
		// 3. uv_offset
		offset = RenderData::InitializeInstanceAttrib(m_vao, 3, 2, offset);

		glBindVertexArray(0);
	}

	void RenderDataSpace::Finalize()
	{
		glDeleteVertexArrays(1, &m_vao);
		glDeleteBuffers(1, &m_instance_vbo);

		// our list doesn't release memory so manually delete them
		m_batch_data_list.ClearDelete();
		m_batch_data_list.Finalize();
	}

	void RenderDataSpace::Clear()
	{
		for (int i = 0; i < m_batch_data_list.GetCount(); i++)
		{
			BatchData* batch_data = (BatchData*)m_batch_data_list[i];
			batch_data->count = 0;
		}
	}

	void RenderDataSpace::Add(const SpriteConfigOutput& config, SpaceAccent space_accent, const Matrix4x4& transform)
	{
		RenderSystem* render_system = g_global_context.m_render_system;
		Vector2 camera_offset = render_system->GetCamera().GetOffset();
		BatchData* batch_data = GetBatchDataOfKey({ config.texture_id, space_accent, config.layer });
		FOO_ASSERT(batch_data->count < MAX_INSTANCES_PER_BRANCH - 1, "Space Render Data Overflow!");
		batch_data->data_instance[batch_data->count] = {
			config.size,
			Vector2{
				transform[0][3] + config.offset.x - camera_offset.x,
				transform[1][3] + config.offset.y - camera_offset.y
			},
			config.uv_size,
			config.uv_offset
		};
		++batch_data->count;
	}

	void RenderDataSpace::Draw()
	{
		glBindVertexArray(m_vao);
		g_global_context.m_shader_manager->SetShader(ShaderManager::ShaderName::SPACE);
		for (int i = 0; i < m_batch_data_list.GetCount(); i++)
		{
			// TODO: sorted
			BatchData* batch_data = (BatchData*)m_batch_data_list[i];
			DrawBatch(batch_data);
		}
	}

	void RenderDataSpace::DrawBatch(const BatchData* batch_data)
	{
		ShaderManager* shader_manager = g_global_context.m_shader_manager;
		shader_manager->SetTexture("uSampler", batch_data->key.texture_id, 0);
		// shader_manager->SetVec4("uColorDefault", g_space_config.GetColorAccent(SpaceId::SPACE_DEFAULT, batch_data->key.space_accent));
		shader_manager->SetVec4("uColorA", g_space_config.GetColorAccent(SpaceId::SPACE_A, batch_data->key.space_accent));
		shader_manager->SetVec4("uColorB", g_space_config.GetColorAccent(SpaceId::SPACE_B, batch_data->key.space_accent));

		//shader_manager->SetVec2("uUVSize", batch_data->tile_data_uniform.uv_size);
		//const Vector4 color = g_space_config.GetSpaceProp(batch_data->key.space_id).color;
		//shader_manager->SetVec4("uColor", color);

		glBindBuffer(GL_ARRAY_BUFFER, m_instance_vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, batch_data->count * sizeof(DataInstance), batch_data->data_instance);

		//glBindVertexBuffer(0, m_vertex_vbo, 0, sizeof(Vertex2));
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, batch_data->count);
	}
	RenderDataSpace::BatchData* RenderDataSpace::GetBatchDataOfKey(const BatchKey& key)
	{
		for (int i = 0; i < m_batch_data_list.GetCount(); i++)
		{
			BatchData* batch_data = (BatchData*)m_batch_data_list[i];
			if (batch_data->key == key)
			{
				return batch_data;
			}
		}
		// add a new branch
		BatchData* batch_data = new BatchData();
		batch_data->key = key;
		m_batch_data_list.Add(batch_data);
		return batch_data;
	}
}