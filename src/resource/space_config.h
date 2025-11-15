#pragma once

#include "math/vector4.h"
namespace foo
{
	enum class SpaceId
	{
		SPACE_DEFAULT,
		SPACE_A,
		SPACE_B,
		SPACE_MAX
	};
	// for tiles
	enum class SpaceMask
	{
		SPACE_DEFAULT = 1,
		SPACE_A = 2,
		SPACE_B = 4,
	};
	enum class SpaceAccent
	{
		SPACE_ACCENT_0,
		SPACE_ACCENT_1,
		SPACE_ACCENT_2,
		SPACE_ACCENT_MAX
	};
	struct SpaceProp
	{
		float color_mask{ 0.0f };
		Vector4 color;
		Vector4 color_accent[static_cast<size_t>(SpaceAccent::SPACE_ACCENT_MAX)];
	};
	class SpaceConfig
	{
	public:
		SpaceConfig()
		{
			m_spaces[static_cast<size_t>(SpaceId::SPACE_DEFAULT)] = {
				0.0f,
				Vector4{ 1.0f, 1.0f, 1.0f, 1.0f }
			};
			m_spaces[static_cast<size_t>(SpaceId::SPACE_A)] = {
				// Vector4{ 0.863f, 0.149f, 0.498f, 1.0f },
				0.5f,
				Vector4{ 0.710f, 0.0f, 0.318f, 1.0f }, // #b50051
				{
					// @f12865
					Vector4{ 0.910f, 0.078f, 0.384f, 1.0f }, // #e81462
					Vector4{ 0.788, 0.0f, 0.329f, 1.0f },    // #c90054s
					Vector4{ 0.710f, 0.0f, 0.318f, 1.0f },   // #b50051
				}
			};
			m_spaces[static_cast<size_t>(SpaceId::SPACE_B)] = {
				0.0f,
				// Vector4{ 0.0f, 0.592f, 0.784f, 1.0f }, // #0097C8
				Vector4{ 0.290f,  0.647f, 0.839f, 1.0f }, // 4AA5D6
				{
					// b6e1f8
					Vector4{ 0.41f, 0.77f, 0.96f, 1.0f },
					Vector4{ 0.361f, 0.702f, 0.878f, 1.0f }, // #5CB3E0
					Vector4{ 0.290f,  0.647f, 0.839f, 1.0f }
					// Vector4{ 0.0f, 0.592f, 0.784f, 1.0f }// Vector4{ 0.29f, 0.73f, 0.88f, 1.0f }, // #49B9E1
				}
			};
		}
		const SpaceProp& GetSpaceProp(SpaceId id) const
		{
			return m_spaces[static_cast<size_t>(id)];
		}
		const Vector4& GetColorAccent(SpaceId id, SpaceAccent accent) const
		{
			return m_spaces[static_cast<size_t>(id)].color_accent[static_cast<size_t>(accent)];
		}
	private:
		SpaceProp m_spaces[static_cast<size_t>(SpaceId::SPACE_MAX)];
	};

	extern SpaceConfig g_space_config;
}