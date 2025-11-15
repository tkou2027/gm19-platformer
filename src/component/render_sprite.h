#pragma once
#include "render_component.h"
#include "render/sprite_config.h"
#include "render/render_system.h"

namespace foo
{
	class RenderSprite : public RenderComponent
	{
	public:
		RenderSprite(const SpriteConfigInput& sprite_config);
		void Update() override;
		void Finalize() override;
		void UpdateRenderData() override;

		const SpriteConfigOutput& GetSpriteConfigOutput() const { return m_sprite_config_output; }
		const SpriteConfigInput& GetSpriteConfigInput() const { return m_sprite_config_input; }
		void SetSpriteConfigInput(const SpriteConfigInput& sprite_config);
		// input??
		void SetSize(const Vector2& size);
		void SetColor(const Vector4& color);
		// state
		void SetFlipX(bool flip_x);
		bool GetFlipX() const { return m_sprite_state.flip_x; }
		void SetUVOffset(const Vector2& uv) { m_sprite_config_input.uv_offset = uv; } // TODO!!

		// animation
		void SetPlaying(bool playing, int frame = -1);
	private:
		// from game object
		SpriteConfigInput m_sprite_config_input{};
		// to render system
		SpriteConfigOutput m_sprite_config_output{};
		// internal states
		SpriteState m_sprite_state{};
	};
}