#pragma once
#include "math/vector2.h"
#include "math/matrix4x4.h"
#include "util/list.h"
#include "sprite_config.h"
#include "resource/space_config.h"
namespace foo
{
	class RenderDataTileMap
	{
	public:
		struct TileDataUniform
		{
			Vector2 size{ 1.0f, 1.0f };
			Vector2 uv_size{ 1.0f, 1.0f };
		};
		struct TileDataInstance
		{
			Vector2 offset{ 1.0f, 1.0f };
			Vector2 uv_offset{ 0.1f, 0.1f };
		};
		void Initialize();
		void Finalize();
		void Clear();

		void Add(const SpriteConfigOutput& config, SpaceId space_id, const Matrix4x4& transform);
		void Draw();
		void DrawMask();
	private:
		static constexpr int MAX_BATCHES{ 8 };
		// TODO: should be 1024, 42 * 23, +1 when not alligned, 41 * 24 = 984
		static constexpr int MAX_TILES_PER_BRANCH{ 1024 };
		struct TileBatchKey
		{
			unsigned int texture_id{ 0 };
			RenderLayer render_layer;
			SpaceId space_id{ SpaceId::SPACE_DEFAULT };
			friend bool operator==(const TileBatchKey& lhs, const TileBatchKey& rhs)
			{
				return lhs.texture_id == rhs.texture_id
					&& lhs.render_layer == rhs.render_layer
					&& lhs.space_id == rhs.space_id;
			}
		};
		struct TileBatchData
		{
			TileBatchKey key;
			int count{ 0 };
			TileDataUniform tile_data_uniform;
			TileDataInstance tile_data_instance[MAX_TILES_PER_BRANCH];
		};

		void DrawBatch(const TileBatchData* batch_data, bool draw_mask = false);
		TileBatchData* GetBatchDataOfKey(const TileBatchKey& key);

		List m_batch_data_list; // list of batches
		unsigned int m_vao{ 0 };
		unsigned int m_instance_vbo{ 0 };
	};
}