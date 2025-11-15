#include "render_data_batch.h"

#include "platform/graphics_gl.h"
#include "platform/assert.h"
#include "global_context.h"
#include "render_system.h"
#include "texture.h"

#include "sprite_config.h"
#include "resource/render_layer.h"
#include "shaders.h"

namespace foo
{
	struct Vertex
	{
		Vector3 position;	// ç¿ïW
	};

	void RenderDataBatch::Initialize()
	{
		m_batch_data_list.Initialize(MAX_BATCHES);

		// buffer
		// create vao ====
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		// vertex
		glGenBuffers(1, &m_vertex_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertex_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, 0, GL_STATIC_DRAW);
		// vertex attributes ====
		// 0.position
		glEnableVertexArrayAttrib(m_vao, 0);
		glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
		glVertexAttribBinding(0, 0);
		// vertex data
		Vertex vertex_position[4] = {
			{{ 1.0f, 0.0f, 0.0f}},
			{{ 0.0f, 0.0f, 0.0f}},
			{{ 1.0f, 1.0f, 0.0f}},
			{{ 0.0f, 1.0f, 0.0f}}
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_position), vertex_position, GL_STATIC_DRAW);
		glVertexArrayVertexBuffer(m_vao, 0, m_vertex_vbo, 0, sizeof(Vertex));


		// instance
		glGenBuffers(1, &m_instance_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_instance_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(SpriteData) * MAX_SPRITES_PER_BRANCH, 0, GL_DYNAMIC_DRAW);
		glVertexArrayVertexBuffer(m_vao, 1, m_instance_vbo, 0, sizeof(SpriteData));
		// instance attribute ====
		unsigned int offset{ 0 };
		// 1.2.transform matrix
		// 1.scale and rotation
		offset = InitializeInstanceAttrib(m_vao, 1, 4, offset);
		// 2.translate
		offset = InitializeInstanceAttrib(m_vao, 2, 4, offset);
		// 3.uv size
		offset = InitializeInstanceAttrib(m_vao, 3, 2, offset);
		// 4.uv offset
		offset = InitializeInstanceAttrib(m_vao, 4, 2, offset);
		//// 5.center
		//offset = InitializeInstanceAttrib(5, 2, offset);
		// 5.color
		offset = InitializeInstanceAttrib(m_vao, 5, 4, offset);

		glBindVertexArray(0);
	}

	void RenderDataBatch::Finalize()
	{
		// our list doesn't release memory so manually delete them
		for (int i = 0; i < m_batch_data_list.GetCount(); i++)
		{
			BatchData* batch_data = (BatchData*)m_batch_data_list[i];
			delete batch_data;
		}
		m_batch_data_list.Finalize();
	}

	void RenderDataBatch::Clear()
	{
		for (int i = 0; i < m_batch_data_list.GetCount(); i++)
		{
			BatchData* batch_data = (BatchData*)m_batch_data_list[i];
			batch_data->sprite_cnt = 0;
		}
	}

	void RenderDataBatch::Add(const SpriteConfigOutput& config, const Matrix4x4& transform)
	{
		RenderSystem* render_system = g_global_context.m_render_system;
		Vector2 camera_offset = render_system->GetCamera().GetOffset();
		BatchData* batch_data = GetBatchDataOfKey({ config.texture_id, (int)config.layer });
		FOO_ASSERT(batch_data->sprite_cnt < MAX_SPRITES_PER_BRANCH - 1, "Sprite Rende Data Overflow!");
		// no z transform as depth will be done by sorting
		float z = 0.0f; // (float)config.layer / (float)RenderLayer::MAX;
		batch_data->sprite_data[batch_data->sprite_cnt] = SpriteData{
			Vector4{
				transform[0][0] * config.size.x, transform[1][0] * config.size.x,
				transform[0][1] * config.size.y, transform[1][1] * config.size.y
			},
			Vector4{
				transform[0][3] + config.offset.x - camera_offset.x,
				transform[1][3] + config.offset.y - camera_offset.y,
				z,
				1
			},
			config.uv_size,
			config.uv_offset,
			//config.center,
			config.color
		};
		++batch_data->sprite_cnt;
	}

	void RenderDataBatch::Draw()
	{
		//RenderSystem* render_system = g_global_context.m_render_system;
		glBindVertexArray(m_vao);
		// bucket sort
		BatchData* batches_sorted[MAX_BATCHES];
		SortBatches(batches_sorted);

		// end sort
		// TODO: better order
		for (int i = 0; i < m_batch_data_list.GetCount(); i++)
		{
			// sorted
			BatchData* batch_data = (BatchData*)batches_sorted[i];
			if (batch_data->key.order != (int)RenderLayer::BACKGROUND_SPACE)
			{
				DrawBatch(batch_data);
			}
		}
	}

	void RenderDataBatch::DrawPlayer()
	{
		// TODO
		glBindVertexArray(m_vao);
		// TODO: better order
		for (int i = 0; i < m_batch_data_list.GetCount(); i++)
		{
			BatchData* batch_data = (BatchData*)m_batch_data_list[i];
			if (batch_data->key.order == (int)RenderLayer::PLAYER)
			{
				DrawBatch(batch_data);
			}
		}
		for (int i = 0; i < m_batch_data_list.GetCount(); i++)
		{
			BatchData* batch_data = (BatchData*)m_batch_data_list[i];
			if (batch_data->key.order == (int)RenderLayer::ITEM)
			{
				DrawBatch(batch_data);
			}
		}
	}

	int RenderDataBatch::InitializeInstanceAttrib(unsigned int vao, unsigned int attrib, unsigned int size, unsigned int offset)
	{
		unsigned int data_size{ 0 };
		switch (size)
		{
		case 2:
		{
			data_size = sizeof(Vector2);
			break;
		}
		case 3:
		{
			data_size = sizeof(Vector3);
			break;
		}
		case 4:
		{
			data_size = sizeof(Vector4);
			break;
		}
		default:
		{
			data_size = size * sizeof(float); // TODO
		}
		}

		glEnableVertexArrayAttrib(vao, attrib);
		glVertexAttribFormat(attrib, size, GL_FLOAT, GL_FALSE, offset);
		glVertexAttribBinding(attrib, 1);
		glVertexBindingDivisor(attrib, 1); // per instance

		return offset + data_size;
	}

	void RenderDataBatch::DrawBatch(const BatchData* batch_data)
	{
		g_global_context.m_texture->SetTexture(batch_data->key.texture_id);

		glBindBuffer(GL_ARRAY_BUFFER, m_instance_vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, batch_data->sprite_cnt * sizeof(SpriteData), batch_data->sprite_data);


		glBindVertexBuffer(0, m_vertex_vbo, 0, sizeof(Vertex));
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, batch_data->sprite_cnt);

	}

	RenderDataBatch::BatchData* RenderDataBatch::GetBatchDataOfKey(const BatchKey& key)
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
	void RenderDataBatch::SortBatches(BatchData* (&batches)[MAX_BATCHES])
	{
		// MAX_LAYER * MAX_BATCHES to MAX_LAYER + MAX_BATCHES * 2
		// with extra MAX_BATCHES space
		// but maybe this is not necessary...
		int count[MAX_BATCHES]{ 0 };
		for (int i = 0; i < m_batch_data_list.GetCount(); i++)
		{
			BatchData* batch_data = (BatchData*)m_batch_data_list[i];
			++count[batch_data->key.order];
		}
		int prev_batch_cnt = count[0];
		count[0] = 0;
		for (int layer = 1; layer < static_cast<int>(RenderLayer::MAX); layer++)
		{
			int acc = count[layer - 1] + prev_batch_cnt;
			prev_batch_cnt = count[layer];
			count[layer] = acc;
		}
		for (int i = 0; i < m_batch_data_list.GetCount(); i++)
		{
			BatchData* batch_data = (BatchData*)m_batch_data_list[i];
			int layer = batch_data->key.order;
			batches[count[layer]++] = batch_data;
		}
	}
}