#include "background.h"
#include "component/render_sprite.h"

namespace foo
{
	Background::Background(const std::string& texture_path)
	{
		SpriteConfigInput sprite_config{};
		sprite_config.texture_path = texture_path;
		sprite_config.size = { (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
		sprite_config.layer = RenderLayer::BACKGROUND;
		SetRenderComponent(new RenderSprite(sprite_config));
	}
}