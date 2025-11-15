#pragma once
#include "game_object.h"
#include "math/vector2.h"
#include "math/transform2d.h"
#include "math/trail_shape.h"
#include "resource/space_config.h"
#include "render/sprite_config.h"

namespace foo
{
	class TrailObject;
	class TrailManager;
	class RenderSprites;
	class Blade : public GameObject
	{
	public:
		Blade(SpaceId space_id, const Vector2& size);
		void Update() override;
		// movements
		void StartSwing();
		void EndSwing();
		void SetSize(const Vector2 size);
		const Vector2& GetSize() const { return m_size; }
		const TrailObject* GetTrailObject() const { return m_trail_object; }
	private:
		void UpdateSwing();
		// visual
		SpriteState m_sprite;
		RenderSprites* m_sprite_renderer{ nullptr };
		Vector2 m_size{ 120.0f, 40.0f };
		// swing
		bool m_swing_active{ false };

		Transform m_center_transform; // center local transform
		Transform2D m_swing_start;
		Transform2D m_swing_end;

		TrailObject* m_trail_object{ nullptr };
		TrailManager* m_trail_manager{ nullptr };
		SpaceId m_space_id{ SpaceId::SPACE_DEFAULT };
	};
}