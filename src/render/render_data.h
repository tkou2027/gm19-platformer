#pragma once
#include "util/list.h"
#include "math/vector2.h"
#include "math/vector4.h"
#include "math/matrix4x4.h"
#include "math/transform2d.h"
#include "sprite_config.h"

namespace foo
{
	class RenderData
	{
	public:
		struct SpriteData
		{
			Vector4 mat_scale_rotation; // m00, m10, m01, m11 of transform matrix (column order)
			Vector4 mat_position; // position of transform matrix
			Vector2 uv_size{ 1.0f, 1.0f };
			Vector2 uv_offset{ 0.0f, 0.0f };
			Vector4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
		};
		void Initialize();
		void Finalize();
		void Clear();
		void Add(const SpriteConfigOutput& config, const Matrix4x4& transform);
		void UpdateDrawData();
		void Draw();
		// draw single layer
		void DrawPlayer(); // for debugging
		void DrawMask();
		void DrawUI();

		static int InitializeInstanceAttrib(unsigned int vao, unsigned int attrib, unsigned int size, unsigned int offset);
	private:
		static constexpr int MAX_BATCHES{ 16 };
		static constexpr int MAX_SPRITES_PER_BRANCH{ 128 };

		struct BatchKey
		{
			unsigned int texture_id{ 0 };
			int order{ 0 };
			friend bool operator==(const BatchKey& lhs, const BatchKey& rhs)
			{
				return lhs.texture_id == rhs.texture_id && lhs.order == rhs.order;
			}
		};
		struct BatchData
		{
			BatchKey key;
			// for continuous memory, don't use list
			int sprite_cnt{ 0 };
			SpriteData sprite_data[MAX_SPRITES_PER_BRANCH]; // list of per-sprite data
		};
		void DrawBatch(const BatchData* batch_data);
		BatchData* GetBatchDataOfKey(const BatchKey& key);
		void SortBatches(BatchData* (&batches)[MAX_BATCHES]);

		List m_batch_data_list; // list of batches
		unsigned int m_vao{ 0 };
		unsigned int m_vertex_vbo{ 0 };
		unsigned int m_instance_vbo{ 0 };

		// sort by layer
		int m_layer_index[static_cast<size_t>(RenderLayer::MAX)];
		BatchData* m_sorted_batches[MAX_BATCHES];
	};
}