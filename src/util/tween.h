#pragma once

#include "object/game_object.h"

namespace foo
{
	class Tween {
	public:
		enum class TWEEN_TYPE : int {
			NONE = 0,
			// attributes
			POSITION = 1 << 0,
			POSITION_X = 1 << 1,
			POSITION_Y = 1 << 2,
			SCALE = 1 << 3,
			ROTATION = 1 << 4,
			// for iteration
			MAX = 1 << 5
		};
		enum class TWEEN_FUNCTION
		{
			LINEAR,
			EASE_IN_QUAD,
			EASE_OUT_QUAD,
			EASE_OUT_BOUNCE,
			EASE_IN_OUT_BACK
		};
		Tween() {};
		void Update();

		Tween& Init(GameObject* sprite, float duration,
			TWEEN_FUNCTION func = TWEEN_FUNCTION::LINEAR,
			bool loop = false, bool yoyo = false);
		// rotation...
		Tween& SetTweenTo(TWEEN_TYPE type, float target_x, float target_y = 0.0f);
		Tween& SetTweenToDelta(TWEEN_TYPE type, float delta_x, float delta_y = 0.0f);
		Tween& SetTweenFromTo(TWEEN_TYPE type, float start, float target);
		Tween& SetTweenFromTo(TWEEN_TYPE type, const Vector2& start, const Vector2& target);
		Tween& SetTweenFromTo(TWEEN_TYPE type, const Vector3& start, const Vector3& target);
		Tween& SetDelay(float delay);
		Tween& SetOffset(float offset, int direction = 1);

		void SetUsed();
		void SetUnused();
		bool GetUse();

		static float EaseFunc(TWEEN_FUNCTION function, float t);
	private:
		TWEEN_FUNCTION m_func{ TWEEN_FUNCTION::LINEAR };
		// loop
		bool m_loop{ false };
		bool m_yoyo{ false };
		// time
		float m_duration{ 0.0f };
		float m_inv_duration{ 0.0f };
		// timer counter
		float m_time_count{ 0.0f };
		float m_time_step{ 1.0f };
		int m_time_direction{ 1 };
		// delay
		float m_delay{ 0.0f };
		float m_delay_count{ 0.0f };
		// sprite
		GameObject* m_object{ nullptr };
		// bits
		int m_type{ 0 };
		int m_to_flag{ 0 }; // if start should be calculated when delay end
		int m_delta_flag{ 0 }; // if target shoud be calucated when delay end
		// values
		Transform m_transform_start;
		Transform m_transform_target;
		//Transform m_uv_transform_start;
		//Transform m_uv_transform_target;

		bool m_updated{ false };
		bool m_use{ false };

		void UpdateInit();
	};

	class TweenPool {
	public:
		Tween* Create();
		Tween& GetTween(int id);
		void Update();
	private:
		static const int POOL_SIZE{ 256 };
		Tween m_tweens[POOL_SIZE];
	};

}