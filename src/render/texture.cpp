#include "texture.h"

#include <sstream>
#include "global_context.h"
#include "render/shaders.h"
#include "platform/graphics_gl.h"
#include "platform/texture_loader.h"
#include "platform/assert.h"

namespace foo
{
	void Texture::Initialize() {}
	void Texture::Finalize()
	{
		for (TextureData& texture_data : m_textures)
		{
			glDeleteTextures(1, &texture_data.texture_id);
		}
	}
	unsigned Texture::LoadTexture(const std::string& filename)
	{
		// check if aleady loaded
		unsigned int texture_id = GetTextureIdLoaded(filename);
		if (texture_id != 0)
		{
			return texture_id;
		}

		// otherwise load new texture
		FOO_ASSERT(m_textures_count <= MAX_TEXTIRE - 1, "Exceeds maximun textures!\n");
		texture_id = TextureLoader::LoadTexture(filename);
		m_textures[m_textures_count] = TextureData{ filename, texture_id };
		m_textures_count++;

		// texture settings
		glBindTexture(GL_TEXTURE_2D, texture_id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //  GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //  GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);

		return texture_id;
	}

	void Texture::UnloadTexture(unsigned int texture_id)
	{
		glDeleteTextures(1, &texture_id);
	}

	void Texture::SetTexture(unsigned int texture_id)
	{
		unsigned int shader_program_id = foo::g_global_context.m_shader_manager->GetShaderProgramId();
		if (texture_id == 0)
		{
			glUniform1f(glGetUniformLocation(shader_program_id, "uTextureEnable"), 0.0f);
		}
		else
		{
			glUniform1f(glGetUniformLocation(shader_program_id, "uTextureEnable"), 1.0f);
			glBindTexture(GL_TEXTURE_2D, texture_id);
		}
	}
	unsigned int Texture::GetTextureIdLoaded(const std::string& filename)
	{

		for (const TextureData& texture_data : m_textures)
		{
			if (texture_data.filename == filename)
			{
				return texture_data.texture_id;
			}
		}
		return 0;
	}
}

