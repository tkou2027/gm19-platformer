#pragma once
#include "game_object.h"
#include "render/sprite_config.h"

namespace foo
{
	class Hint : public GameObject
	{
	public:
		Hint();
		void Update() override;
		enum class HintType
		{
			MOVE,
			CHARGE,
			TURN,
			ATTACK,
			TAKE,
			MAX
		};
		void ShowHint(HintType type, const Vector2& position);
		void StopHint(HintType type);
	private:
		enum class HintState
		{
			NONE,
			FADE_IN,
			ACTIVE,
			FADE_OUT
		};
		struct HintConfig
		{
			int fade_in_countdown{ 60 };
			int active_countdown{ 180 };
			int fade_out_countdown{ 30 };
		};
		static constexpr int NUM_SPRITES{ 2 };

		SpriteState m_sprites[NUM_SPRITES];
		Transform m_sprite_transform[NUM_SPRITES];
		bool m_hint_end[(size_t)HintType::MAX];

		void EnterFadeIn();
		void EnterFadeOut();
		void EndHint();
		int m_countdown{ 0 };
		HintConfig m_config{};
		HintState m_hint_state{ HintState::NONE };
		HintType m_hint_type{ HintType::MAX };
	};
}