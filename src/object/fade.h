#pragma once
#include "game_object.h"
#include "render/sprite_config.h"
namespace foo
{
	class ParticleSystem;
	class Fade : public GameObject
	{
	public:
		struct FadeConfig
		{
			int duration{ 20 };
			SpaceId space_id{ SpaceId::SPACE_A };
			bool mask{ false };
		};
		Fade(const FadeConfig& config);
		void Update() override;
		void StartFadeIn(bool fade_in_out = false);
		void StartFadeOut(bool fade_out_in = false);
		bool IsIdle() const;
	private:
		static constexpr int NUM_SPRITES{ 2 };
		enum class FadeState
		{
			IDLE,
			FADE_IN,
			FADE_OUT
		};
		void EnterIdle();
		void UpdateFadeIn();
		void UpdateFadeOut();
		void UpdateTransform();
		int m_countdown{ 0 };
		SpriteState m_sprites[NUM_SPRITES];
		FadeState m_state{ FadeState::IDLE };
		FadeConfig m_config;
		bool m_fade_in_out{ false };
		bool m_fade_out_in{ false };
	};
}