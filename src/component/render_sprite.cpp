#include "render_sprite.h"

#include "global_context.h"
#include "render/texture.h"
#include "object/game_object.h"

namespace foo
{
	RenderSprite::RenderSprite(const SpriteConfigInput& sprite_config_input)
	{
		m_sprite_config_input = sprite_config_input;
		m_sprite_state.Initialize(m_sprite_config_input);
		m_sprite_state.GetConfig(m_sprite_config_output);
	}

	void RenderSprite::UpdateRenderData()
	{
		m_sprite_state.GetConfig(m_sprite_config_output);
		RenderSystem* render_system = g_global_context.m_render_system;
		const Matrix4x4& mat{
			m_sprite_state.transform ? m_sprite_state.transform->GetMatrix() : m_object->GetTransform().GetMatrix()
		};
		// TODO
		if (m_sprite_config_output.layer == RenderLayer::BACKGROUND_SPACE)
		{
			render_system->GetSpaceData().Add(
				m_sprite_config_output,
				m_sprite_config_input.space_accent,
				mat
			);
			return;
		}
		render_system->GetRenderData().Add(m_sprite_config_output, mat);
	}

	void RenderSprite::Update()
	{
		m_sprite_state.Update();
	}

	void RenderSprite::Finalize()
	{
		// TODO: release texture
	}

	void RenderSprite::SetSpriteConfigInput(const SpriteConfigInput& sprite_config)
	{
		m_sprite_config_input = sprite_config;
		m_sprite_state.Initialize(m_sprite_config_input);
	}

	void RenderSprite::SetSize(const Vector2& size)
	{
		m_sprite_config_input.size = size;
	}

	void RenderSprite::SetColor(const Vector4& color)
	{
		m_sprite_config_input.color = color;
	}

	// uv flip
	void RenderSprite::SetFlipX(bool flip_x)
	{
		m_sprite_state.flip_x = flip_x;
	}
	// animation
	void RenderSprite::SetPlaying(bool playing, int frame)
	{
		m_sprite_state.playing = playing;
		if (frame > 0)
		{
			m_sprite_state.frame = frame;
		}
	}
}

