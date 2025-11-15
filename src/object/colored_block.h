#pragma once
#include "game_object.h"
#include "component/render_sprite.h"

namespace foo
{
	class ColoredBlock : public GameObject
	{
	public:
		ColoredBlock(float pos_x, float pos_y, float width, float height, const Vector4& color)
		{
			SpriteConfigInput sprite_config{};
			sprite_config.color = color;
			sprite_config.size = { width, height };
			SetRenderComponent(new RenderSprite(sprite_config));

			Vector3 pos = m_transform.GetPosition();
			pos.x = pos_x;
			pos.y = pos_y;
			m_transform.SetPosition(pos);
		}
	};
}