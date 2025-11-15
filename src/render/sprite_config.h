#pragma once

#include <string>
#include "math/vector2.h"
#include "math/vector4.h"
#include "resource/render_layer.h"
#include "resource/space_config.h"
#include "component/transform.h"
#include "platform/assert.h"

namespace foo
{
	// what objects passes to render components
	struct SpriteConfigInput
	{
		// textures
		std::string texture_path{};
		unsigned int texture_id{ 0 }; // to be computed
		// color
		Vector4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
		SpaceId space_id{ SpaceId::SPACE_DEFAULT };
		SpaceAccent space_accent{ SpaceAccent::SPACE_ACCENT_0 }; // accent (parallax)
		// ordering
		RenderLayer layer{ RenderLayer::DEFAULT };
		// transform
		Vector2 size{ 1.0f, 1.0f };
		Vector2 offset{};
		// uv transform
		Vector2 uv_size{ 1.0f, 1.0f };
		Vector2 uv_offset{ 0.0f, 0.0f };
		Vector2 uv_size_per_frame{ 1.0f, 1.0f }; // to be computed
		// uv animation
		int frame_cols{ 1 };
		int frame_rows{ 1 };
		int frame_total{ 1 };
		int frame_default{ 0 };
		int play_speed_scale{ 1 };
		bool play_loop{ false };
	};

	// what will be processed by render system
	struct SpriteConfigOutput
	{
		// textures
		unsigned int texture_id{ 0 };
		Vector4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
		// ordering
		RenderLayer layer{ RenderLayer::DEFAULT };
		// transform
		Vector2 size{ 1.0f, 1.0f };
		Vector2 offset{};
		// uv transform
		Vector2 uv_size{ 1.0f, 1.0f };
		Vector2 uv_offset{ 0.0f, 0.0f };
	};

	// update helper ====
	class SpriteState
	{
	public:
		bool active{ true };
		int frame{ 0 };
		int play_count{ 0 };
		bool playing{ true };
		bool flip_x{ false };
		// overwrite input
		Vector2 size{ 1.0f, 1.0f };
		Vector2 offset{};
		Vector2 uv_offset{}; // uv scrolling, not compatible with griding and fliping
		Vector4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
		SpaceId space_id{ SpaceId::SPACE_DEFAULT };
		RenderLayer layer{ RenderLayer::DEFAULT };
		// referencing 
		const SpriteConfigInput* config_input{ nullptr };
		Transform* transform{ nullptr };

		void Initialize(const SpriteConfigInput& input)
		{
			active = true;
			color = input.color;
			space_id = input.space_id;
			layer = input.layer;
			size = input.size;
			offset = input.offset;
			uv_offset = input.uv_offset;
			SetConfigInput(input);
		}

		void SetConfigInput(const SpriteConfigInput& input)
		{
			config_input = &input;
			frame = input.frame_default % input.frame_total;
			play_count = 0;
			uv_offset = input.uv_offset; // ?
			playing = true;
		}

		void Update()
		{
			if (!active)
			{
				return;
			}
			const SpriteConfigInput& input = *config_input;
			if (!playing || input.frame_total <= 1)
			{
				return;
			}

			play_count++;
			if (play_count == input.play_speed_scale)
			{
				play_count = 0;
				if (!input.play_loop && frame + 1 >= input.frame_total)
				{
					playing = false;
					return;
				}
				frame = (++frame) % input.frame_total;
			}
		}

		SpriteConfigOutput GetConfig() const
		{
			SpriteConfigOutput output{};
			GetConfig(output);
			return output;
		}

		void GetConfig(SpriteConfigOutput& output) const
		{
			if (!active)
			{
				return;
			}
			const SpriteConfigInput& input = *config_input;
			output.texture_id = input.texture_id;

			if (layer == RenderLayer::SPACE_MASK)
			{
				float color_mask = g_space_config.GetSpaceProp(space_id).color_mask;
				output.color = Vector4{ color_mask, color_mask, color_mask, 1.0f };
			}
			else
			{
				output.color = color;// input.color;
			}
			// ordering
			output.layer = layer;// input.layer;
			// transform
			output.size = size; // input.size;
			output.offset = offset;

			// uv transform
			// split tile sheet
			output.uv_size = input.uv_size_per_frame;
			// frame
			int frame_valid = frame % input.frame_total;
			int col = frame_valid % input.frame_cols;
			int row = frame_valid / input.frame_cols;
			output.uv_offset = Vector2{
				col * input.uv_size_per_frame.x + uv_offset.x, // input.uv_offset.x,
				row * input.uv_size_per_frame.y + uv_offset.y // input.uv_offset.y
			};
			if (flip_x)
			{
				output.uv_offset.x = output.uv_offset.x + input.uv_size_per_frame.x;
				output.uv_size.x = -input.uv_size_per_frame.x;
			}
			// uv scrolling, not compatible with griding and flipping
			output.uv_offset = output.uv_offset;
		}
	};

	class SpriteStateList
	{
	public:
		void Add(const SpriteState& sprite_state)
		{
			FOO_ASSERT(m_count < MAX_SPRITES - 1, "Sprite parts overflow");
			m_data[m_count] = sprite_state;
			m_count++;
		}
		int CetCount() const
		{
			return m_count;
		}
	private:
		static constexpr int MAX_SPRITES{ 8 }; // max parts
		SpriteState m_data[MAX_SPRITES];
		int m_count{ 0 };
	};
}