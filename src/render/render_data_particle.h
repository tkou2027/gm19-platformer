#pragma once
#include "math/vector2.h"
#include "math/matrix4x4.h"
#include "util/list.h"
#include "util/particle.h"
#include "resource/space_config.h"
#include "resource/render_layer.h"

namespace foo
{
	class RenderDataParticle
	{
	public:
		void Initialize();
		void Finalize();
		void Clear();

		void Add(const Particle& particle);
		void Draw();
		void DrawMask();
	private:
		static constexpr int MAX_BATCHES{ 4 };
		static constexpr int MAX_INSTANCES_PER_BRANCH{ 2048 };
		struct BatchKey
		{
			RenderLayer render_layer;
			friend bool operator==(const BatchKey& lhs, const BatchKey& rhs)
			{
				return lhs.render_layer == rhs.render_layer;
			}
		};
		struct DataInstance
		{
			Vector2 size{ 1.0f, 1.0f };
			Vector2 offset{ 1.0f, 1.0f };
			Vector2 color{ 0.0f, 1.0f };
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
		Vector2 EncodeColor(const Particle& particle) const;

		List m_batch_data_list; // list of batches
		unsigned int m_vao{ 0 };
		unsigned int m_instance_vbo{ 0 };
	};
}