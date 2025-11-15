#include "render_data_particle.h"

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
	void RenderDataParticle::Initialize()
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
		// 3. color
		offset = RenderData::InitializeInstanceAttrib(m_vao, 2, 2, offset);
		glBindVertexArray(0);
	}

	void RenderDataParticle::Finalize()
	{
		glDeleteVertexArrays(1, &m_vao);
		glDeleteBuffers(1, &m_instance_vbo);

		// our list doesn't release memory so manually delete them
		m_batch_data_list.ClearDelete();
		m_batch_data_list.Finalize();
	}

	void RenderDataParticle::Clear()
	{
		for (int i = 0; i < m_batch_data_list.GetCount(); i++)
		{
			BatchData* batch_data = (BatchData*)m_batch_data_list[i];
			batch_data->count = 0;
		}
	}

	void RenderDataParticle::Add(const Particle& particle)
	{
		RenderSystem* render_system = g_global_context.m_render_system;
		Vector2 camera_offset = render_system->GetCamera().GetOffset();
		RenderLayer layer = particle.is_mask ? RenderLayer::SPACE_MASK : RenderLayer::AFTER_EFFECT; // TODO
		BatchData* batch_data = GetBatchDataOfKey({ layer });
		FOO_ASSERT(batch_data->count <= MAX_INSTANCES_PER_BRANCH - 1, "Particle Render Data Overflow!");
		batch_data->data_instance[batch_data->count] = {
			particle.size,
			particle.position - camera_offset,
			EncodeColor(particle)
		};
		++batch_data->count;
	}

	void RenderDataParticle::Draw()
	{
		glBindVertexArray(m_vao);
		g_global_context.m_shader_manager->SetShader(ShaderManager::ShaderName::PARTICLE);
		for (int i = 0; i < m_batch_data_list.GetCount(); i++)
		{
			// TODO: sorted
			BatchData* batch_data = (BatchData*)m_batch_data_list[i];
			if (batch_data->key.render_layer != RenderLayer::SPACE_MASK)
			{
				DrawBatch(batch_data);
			}
		}
	}

	void RenderDataParticle::DrawMask()
	{
		glBindVertexArray(m_vao);
		g_global_context.m_shader_manager->SetShader(ShaderManager::ShaderName::PARTICLE);
		for (int i = 0; i < m_batch_data_list.GetCount(); i++)
		{
			// TODO: sorted
			BatchData* batch_data = (BatchData*)m_batch_data_list[i];
			if (batch_data->key.render_layer == RenderLayer::SPACE_MASK)
			{
				DrawBatch(batch_data);
			}
		}
	}

	void RenderDataParticle::DrawBatch(const BatchData* batch_data)
	{
		ShaderManager* shader_manager = g_global_context.m_shader_manager;
		glBindBuffer(GL_ARRAY_BUFFER, m_instance_vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, batch_data->count * sizeof(DataInstance), batch_data->data_instance);

		//glBindVertexBuffer(0, m_vertex_vbo, 0, sizeof(Vertex2));
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, batch_data->count);
	}
	RenderDataParticle::BatchData* RenderDataParticle::GetBatchDataOfKey(const BatchKey& key)
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
	Vector2 RenderDataParticle::EncodeColor(const Particle& particle) const
	{
		float mask = g_space_config.GetSpaceProp(particle.space_id).color_mask;
		float is_mask = particle.is_mask ? 1.0f : 0.0f;
		//switch (particle.space_id)
		//{
		//case SpaceId::SPACE_A:
		//{
		//	color = 0.5f;
		//	break;
		//}
		//case SpaceId::SPACE_B:
		//{
		//	color = 1.0f;
		//	break;
		//}
		//}
		return { mask, is_mask }; // TODO: opacity?
	}
}