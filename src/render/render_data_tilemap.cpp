#include "render_data_tilemap.h"

#include "platform/graphics_gl.h"
#include "platform/assert.h"
#include "global_context.h"
#include "render_system.h"
#include "render_data.h"
#include "shaders.h"
#include "texture.h"
#include "resource/render_layer.h"

namespace foo
{
	void RenderDataTileMap::Initialize()
	{
		m_batch_data_list.Initialize(MAX_BATCHES);

		// buffer
		// create vao ====
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		// instance
		glGenBuffers(1, &m_instance_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_instance_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(TileDataInstance) * MAX_TILES_PER_BRANCH, 0, GL_DYNAMIC_DRAW);
		glVertexArrayVertexBuffer(m_vao, 1, m_instance_vbo, 0, sizeof(TileDataInstance));
		// instance attribute ====
		unsigned int offset{ 0 };
		// 0. offset
		offset = RenderData::InitializeInstanceAttrib(m_vao, 0, 2, offset);
		// 1. uv_offset
		offset = RenderData::InitializeInstanceAttrib(m_vao, 1, 2, offset);

		glBindVertexArray(0);
	}

	void RenderDataTileMap::Finalize()
	{
		glDeleteVertexArrays(1, &m_vao);
		glDeleteBuffers(1, &m_instance_vbo);

		// our list doesn't release memory so manually delete them
		m_batch_data_list.ClearDelete();
		m_batch_data_list.Finalize();
	}

	void RenderDataTileMap::Clear()
	{
		for (int i = 0; i < m_batch_data_list.GetCount(); i++)
		{
			TileBatchData* batch_data = (TileBatchData*)m_batch_data_list[i];
			batch_data->count = 0;
		}
	}

	void RenderDataTileMap::Add(const SpriteConfigOutput& config, SpaceId space_id, const Matrix4x4& transform)
	{
		RenderSystem* render_system = g_global_context.m_render_system;
		Vector2 camera_offset = render_system->GetCamera().GetOffset();
		TileBatchData* batch_data = GetBatchDataOfKey({ config.texture_id, config.layer, space_id });
		FOO_ASSERT(batch_data->count <= MAX_TILES_PER_BRANCH - 1, "Tile Map Rende Data Overflow!");
		batch_data->tile_data_uniform = {
			config.size, config.uv_size
		};
		batch_data->tile_data_instance[batch_data->count] = {
			Vector2{
				transform[0][3] + config.offset.x - camera_offset.x,
				transform[1][3] + config.offset.y - camera_offset.y
			},
			config.uv_offset
		};
		++batch_data->count;
	}

	void RenderDataTileMap::Draw()
	{
		glBindVertexArray(m_vao);
		g_global_context.m_shader_manager->SetShader(ShaderManager::ShaderName::TILE_MAP);
		for (int i = 0; i < m_batch_data_list.GetCount(); i++)
		{
			// TODO: sorted
			TileBatchData* batch_data = (TileBatchData*)m_batch_data_list[i];
			SpaceId space = batch_data->key.space_id;
			if (space != SpaceId::SPACE_DEFAULT)
			{
				// TODO not mask texture
				continue;
			}
			DrawBatch(batch_data);
		}
	}

	void RenderDataTileMap::DrawMask()
	{
		glBindVertexArray(m_vao);
		g_global_context.m_shader_manager->SetShader(ShaderManager::ShaderName::TILE_MAP);
		for (int i = 0; i < m_batch_data_list.GetCount(); i++)
		{
			// TODO: sorted
			TileBatchData* batch_data = (TileBatchData*)m_batch_data_list[i];
			SpaceId space = batch_data->key.space_id;
			if (space == SpaceId::SPACE_DEFAULT)
			{
				continue;
			}
			DrawBatch(batch_data, true);
		}
	}

	void RenderDataTileMap::DrawBatch(const TileBatchData* batch_data, bool draw_mask)
	{
		g_global_context.m_texture->SetTexture(batch_data->key.texture_id);
		ShaderManager* shader_manager = g_global_context.m_shader_manager;
		shader_manager->SetVec2("uSize", batch_data->tile_data_uniform.size);
		shader_manager->SetVec2("uUVSize", batch_data->tile_data_uniform.uv_size);
		if (draw_mask)
		{
			float color_mask = g_space_config.GetSpaceProp(batch_data->key.space_id).color_mask;
			shader_manager->SetVec4("uColor", Vector4{ color_mask, color_mask, color_mask, 1.0f });
		}
		else
		{
			// TODO: should not use color
			const Vector4 color = g_space_config.GetSpaceProp(batch_data->key.space_id).color;
			shader_manager->SetVec4("uColor", color);
		}
		glBindBuffer(GL_ARRAY_BUFFER, m_instance_vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, batch_data->count * sizeof(TileDataInstance), batch_data->tile_data_instance);
		
		//glBindVertexBuffer(0, m_vertex_vbo, 0, sizeof(Vertex2));
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, batch_data->count);
	}

	RenderDataTileMap::TileBatchData* RenderDataTileMap::GetBatchDataOfKey(const TileBatchKey& key)
	{
		for (int i = 0; i < m_batch_data_list.GetCount(); i++)
		{
			TileBatchData* batch_data = (TileBatchData*)m_batch_data_list[i];
			if (batch_data->key == key)
			{
				return batch_data;
			}
		}
		// add a new branch
		TileBatchData* batch_data = new TileBatchData();
		batch_data->key = key;
		m_batch_data_list.Add(batch_data);
		return batch_data;
	}
}