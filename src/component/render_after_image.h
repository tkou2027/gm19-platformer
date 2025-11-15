#pragma once
#include <string>
#include "render_component.h"
#include "render/sprite_config.h"
#include "render_sprite.h"

namespace foo
{
	class RenderAfterImage : public RenderComponent
	{
	public:
		struct AfterimageConfig
		{
			// after image
			int count{ 4 };
			float time_delay{ 0.05f };
			Vector4 color{ 1.0f, 1.0f, 1.0f, 0.5f};
			RenderLayer layer{ RenderLayer::AFTER_EFFECT };
			RenderSprite* render_sprite;
		};
		RenderAfterImage(const AfterimageConfig& config);
		void Update() override;
		void Finalize() override {}; // TODO release texture

		void UpdateRenderData() override;

		void SetActive(bool active) override;
		void ClearRecords();
	private:
		static constexpr int MAX_RECORDS{ 32 };
		struct AfterImageRecord
		{
			Matrix4x4 transform_mat;
			SpriteConfigOutput sprite_config_output;
			bool valid{ false };
		};
		AfterimageConfig m_config;

		// records
		AfterImageRecord m_records[MAX_RECORDS];
		// recording control
		int m_next_record_index{ 0 };
		float m_countdown{ 0 };
	};
}