#include "render_sprites.h"

#include "global_context.h"
#include "render/render_system.h"
#include "object/game_object.h"

namespace foo
{
	RenderSprites::RenderSprites(SpriteState* sprites, int sprite_num)
		: m_sprites(sprites), m_sprite_num(sprite_num) {}
	void RenderSprites::Update()
	{
		for (int i = 0; i < m_sprite_num; i++)
		{
			m_sprites[i].Update();
		}
	}
	void RenderSprites::UpdateRenderData()
	{
		for (int i = 0; i < m_sprite_num; i++)
		{
			UpdateSpriteRenderData(m_sprites[i]);
		}
	}
	void RenderSprites::UpdateSpriteRenderData(SpriteState& sprite_state)
	{
		if (!sprite_state.active)
		{
			return;
		}
		SpriteConfigOutput sprite_config_output{};
		sprite_state.GetConfig(sprite_config_output);
		RenderSystem* render_system = g_global_context.m_render_system;
		const Matrix4x4& mat{
			sprite_state.transform ? sprite_state.transform->GetMatrix() : m_object->GetTransform().GetMatrix()
		};
		// TODO
		if (sprite_config_output.layer == RenderLayer::BACKGROUND_SPACE)
		{
			render_system->GetSpaceData().Add(
				sprite_config_output,
				sprite_state.config_input->space_accent, // TODO
				mat
			);
			return;
		}
		render_system->GetRenderData().Add(sprite_config_output, mat);
	}
}
