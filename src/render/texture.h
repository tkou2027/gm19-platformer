#pragma once
#include <string>
#include "main.h"
namespace foo
{
	class Texture
	{
	public:
		void Initialize();
		void Finalize();
		unsigned int LoadTexture(const std::string& filename);
		void UnloadTexture(unsigned int texture_id);
		// TODO: set shader
		void SetTexture(unsigned int texture_id);
	private:
		struct TextureData
		{
			std::string filename;
			unsigned int texture_id{ 0 };
			// TODO: texture size
			// TODO: use count
		};
		static constexpr int MAX_TEXTIRE{ 32 };

		unsigned int GetTextureIdLoaded(const std::string& filename);
		TextureData m_textures[MAX_TEXTIRE];
		int m_textures_count{ 0 };
	};
}