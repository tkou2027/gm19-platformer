#pragma once
#include <cstddef> // size_t
#include <string>
#include "math/vector2.h"
#include "math/vector4.h"
#include "math/matrix4x4.h";

namespace foo
{
	class ShaderManager
	{
	public:
		enum class ShaderName
		{
			DEFAULT,
			TILE_MAP,
			TRAIL,
			SPACE,
			PARTICLE,
			COMBINE_REFLECTION,
			POST_PROCESS,
			// numbers
			MAX,
			CURRENT
		};
		void Initialize();
		void Finalize();
		unsigned int GetShaderProgramId() const;
		unsigned int GetShaderProgramId(ShaderName name) const;
		void SetShader(ShaderName name = ShaderName::DEFAULT);
		// set uniform variables
		void SetTransformMatrix(const Matrix4x4& transform) const;
		void SetInt(const std::string& name, int value) const;
		void SetFloat(const std::string& name, float value)  const;
		void SetVec2(const std::string& name, const Vector2& value) const;
		void SetVec4(const std::string& name, const Vector4& value) const;
		void SetTexture(const std::string& sampler_name, unsigned int texture_id, int sampler_value = 0) const;
	private:
		static unsigned int CompileShaderProgram(
			const char* vert, const char* frag
		);
		unsigned int m_shader_ids[(size_t)ShaderName::MAX]{};
		unsigned int m_shader_current{ 0 };
	};
}