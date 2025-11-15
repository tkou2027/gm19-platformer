#pragma once
#include "math/vector2.h"
#include "math/matrix4x4.h"
#include "util/list.h"
#include "sprite_config.h"
#include "resource/space_config.h"

namespace foo
{
	class RenderDataSpace
	{
	public:
		struct DataInstance
		{
			Vector2 size{ 1.0f, 1.0f };
			Vector2 offset{ 1.0f, 1.0f };
			Vector2 uv_size{ 1.0f, 1.0f };
			Vector2 uv_offset{ 0.1f, 0.1f };
		};
		void Initialize();
		void Finalize();
		void Clear();

		void Add(const SpriteConfigOutput& config, SpaceAccent space_accent, const Matrix4x4& transform);
		void Draw();
		void SetTrailTexture(unsigned int texture) { m_trail_texture = texture; }
	private:
		static constexpr int MAX_BATCHES{ 32 };
		static constexpr int MAX_INSTANCES_PER_BRANCH{ 32 };
		struct BatchKey
		{
			unsigned int texture_id{ 0 };
			SpaceAccent space_accent;
			RenderLayer render_layer;
			friend bool operator==(const BatchKey& lhs, const BatchKey& rhs)
			{
				return lhs.texture_id == rhs.texture_id
					&& lhs.space_accent == rhs.space_accent
					&& lhs.render_layer == rhs.render_layer;
			}
		};
		struct BatchData
		{
			BatchKey key;
			int count{ 0 };
			DataInstance data_instance[MAX_INSTANCES_PER_BRANCH];
		};

		void DrawBatch(const BatchData* batch_data);
		// TODO
		BatchData* GetBatchDataOfKey(const BatchKey& key);

		List m_batch_data_list; // list of batches
		unsigned int m_vao{ 0 };
		unsigned int m_instance_vbo{ 0 };
		unsigned int m_trail_texture{ 0 };
	};
}