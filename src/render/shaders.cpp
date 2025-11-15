#include "shaders.h"
#include "platform/graphics_gl.h"

#include "main.h" // screen size
#include "platform/assert.h"

namespace
{
	// default ========
	const char* vertex_shader_default{
		"#version 450\n"
		"precision highp float;\n"

		"uniform mat4 uProjection;\n"
		"uniform mat4 uTransform;\n"

		"layout( location = 0 ) in vec3 inPosition;\n"
		"layout( location = 1 ) in vec4 inColor;\n"
		"layout( location = 2 ) in vec2 inTexCoord;\n"

		"out vec4 vColor;\n"
		"out vec2 vTexCoord;\n"

		"void main() {\n"
		"    vColor = inColor;\n"
		"    vTexCoord = inTexCoord;\n"
		"    gl_Position = vec4(inPosition, 1.0) * uTransform * uProjection;\n"
		"}\n"
	};

	const char* vertex_shader_default_batch{ R"GLSL(
		#version 450 core
		precision highp float;
		
		layout(location = 0) in vec3 inPosition;

		layout(location = 1) in vec4 inMatScaleRotation;
		layout(location = 2) in vec4 inMatTransform;
		layout(location = 3) in vec2 inUVSize;
		layout(location = 4) in vec2 inUVOffset;
		layout(location = 5) in vec4 inColor;
		
		uniform mat4 uProjection;

		out vec4 vColor;
		out vec2 vTexCoord;
		const vec2 tex[4] = vec2[](
		    vec2(1.0, 0.0),
		    vec2(0.0, 0.0),
		    vec2(1.0, 1.0),
		    vec2(0.0, 1.0)
		);
		void main() {
			vColor = inColor;
			vTexCoord = tex[gl_VertexID];
			vTexCoord = vec2(vTexCoord.x * inUVSize.x + inUVOffset.x, vTexCoord.y * inUVSize.y + inUVOffset.y);
			mat4 transform = mat4(
				vec4(inMatScaleRotation.x, inMatScaleRotation.y, 0.0, 0.0),
				vec4(inMatScaleRotation.z, inMatScaleRotation.w, 0.0, 0.0),
				vec4(0.0, 0.0, 1.0, 0.0),
				inMatTransform
			);
			gl_Position = uProjection * transform * vec4(inPosition.x - 0.5, inPosition.y - 0.5, inPosition.z, 1.0);
		}

	)GLSL" };

	const char* vertex_shader_tile_map{ R"GLSL(
		#version 450 core
		precision highp float;

		layout(location = 0) in vec2 inOffset;
		layout(location = 1) in vec2 inUVOffset;

		uniform mat4 uProjection;
		uniform vec2 uSize;
		uniform vec2 uUVSize;
		uniform vec4 uColor;

		out vec4 vColor;
		out vec2 vTexCoord;

		const vec2 pos[4] = vec2[](
		    vec2(0.5, -0.5),
		    vec2(-0.5, -0.5),
		    vec2(0.5, 0.5),
		    vec2(-0.5, 0.5)
		);
		const vec2 tex[4] = vec2[](
		    vec2(1.0, 0.0),
		    vec2(0.0, 0.0),
		    vec2(1.0, 1.0),
		    vec2(0.0, 1.0)
		);
		void main() {
			vColor = uColor;
			vTexCoord = tex[gl_VertexID];
			vTexCoord = vec2(vTexCoord.x * uUVSize.x + inUVOffset.x, vTexCoord.y * uUVSize.y + inUVOffset.y);
			mat4 transform = mat4(
				vec4(uSize.x, 0.0, 0.0, 0.0),
				vec4(0.0, uSize.y, 0.0, 0.0),
				vec4(0.0, 0.0, 1.0, 0.0),
				vec4(inOffset.x, inOffset.y, 0.0, 1.0)
			);
			gl_Position = uProjection * transform * vec4(pos[gl_VertexID], 0.0, 1.0);
		}
	)GLSL" };

	const char* vertex_shader_particle{ R"GLSL(
		#version 450 core
		precision highp float;

		layout(location = 0) in vec2 inSize;
		layout(location = 1) in vec2 inOffset;
		layout(location = 2) in vec2 inColor;

		uniform mat4 uProjection;
		//uniform vec4 uColorA;
		//uniform vec4 uColorB;

		out vec4 vColor;
		out vec2 vTexCoord;

		const vec2 pos[4] = vec2[](
		    vec2(0.5, -0.5),
		    vec2(-0.5, -0.5),
		    vec2(0.5, 0.5),
		    vec2(-0.5, 0.5)
		);
		const vec2 tex[4] = vec2[](
		    vec2(1.0, 0.0),
		    vec2(0.0, 0.0),
		    vec2(1.0, 1.0),
		    vec2(0.0, 1.0)
		);
		void main() {
			// TODO: uniform
			vec4 uColorA = vec4(0.710, 0.0, 0.318, 1.0);
			vec4 uColorB = vec4(0.0, 0.592, 0.784, 1.0);

			vec4 uColorAB = uColorA - uColorB;
			vec4 color = uColorB + step(0.4, inColor.x) * uColorAB;
			vec4 colorMask = vec4(inColor.x, inColor.x, inColor.x, 1.0);
			vColor = mix(color, colorMask, step(0.5, inColor.y));
			vTexCoord = tex[gl_VertexID];   
			mat4 transform = mat4(
				vec4(inSize.x, 0.0, 0.0, 0.0),
				vec4(0.0, inSize.y, 0.0, 0.0),
				vec4(0.0, 0.0, 1.0, 0.0),
				vec4(inOffset.x, inOffset.y, 0.0, 1.0)
			);
			gl_Position = uProjection * transform * vec4(pos[gl_VertexID], 0.0, 1.0);
		}
	)GLSL" };

	const char* fragment_shader_default{ R"GLSL(
		#version 450
		precision highp float;

		uniform sampler2D uSampler;
		uniform float uTextureEnable;

		in vec4 vColor;
		in vec2 vTexCoord;

		out vec4 outColor;

		void main() {
			// outColor = vColor * texture(uSampler, vTexCoord);
			outColor = vColor * mix(vec4(1.0), texture(uSampler, vTexCoord), uTextureEnable);
			// outColor = vec4(vTexCoord, 0.0, 1.0);
			//if (uTextureEnable > 0.5) {
			//    outColor = vColor;
			//} else {
			//    outColor = vColor* texture(uSampler, vTexCoord);
			//}
			// outColor = vec4(uTextureEnable, 0.0, 0.0, 1.0);
		}
	)GLSL" };

	const char* vertex_shader_space{ R"GLSL(
		#version 450 core
		precision highp float;

		layout(location = 0) in vec2 inSize;
		layout(location = 1) in vec2 inOffset;
		layout(location = 2) in vec2 inUVSize;
		layout(location = 3) in vec2 inUVOffset;
		
		uniform mat4 uProjection;

		out vec4 vColor;
		out vec2 vTexCoord;
		const vec2 pos[4] = vec2[](
		    vec2(0.5, -0.5),
		    vec2(-0.5, -0.5),
		    vec2(0.5, 0.5),
		    vec2(-0.5, 0.5)
		);
		const vec2 tex[4] = vec2[](
		    vec2(1.0, 0.0),
		    vec2(0.0, 0.0),
		    vec2(1.0, 1.0),
		    vec2(0.0, 1.0)
		);
		void main() {
			vTexCoord = tex[gl_VertexID];
			vTexCoord = vec2(vTexCoord.x * inUVSize.x + inUVOffset.x, vTexCoord.y * inUVSize.y + inUVOffset.y);
			mat4 transform = mat4(
				vec4(inSize.x, 0.0, 0.0, 0.0),
				vec4(0.0, inSize.y, 0.0, 0.0),
				vec4(0.0, 0.0, 1.0, 0.0),
				vec4(inOffset.x, inOffset.y, 0.0, 1.0)
			);
			gl_Position = uProjection * transform * vec4(pos[gl_VertexID], 0.0, 1.0);
		}

	)GLSL" };

	const char* fragment_shader_space{ R"GLSL(
		#version 450
		precision highp float;

		const vec2 screenResolution = vec2(1920.0, 1080.0);

		uniform sampler2D uSampler;
		uniform sampler2D uSamplerSpace;
		uniform vec4 uColorA;
		uniform vec4 uColorB;
		in vec2 vTexCoord;

		out vec4 outColor;

		void main() {
			// outColor = texture(uSampler, vTexCoord);
			vec4 space =  texture(uSamplerSpace,
				vec2(gl_FragCoord.x / screenResolution.x, gl_FragCoord.y / screenResolution.y));
			vec4 object = texture(uSampler, vTexCoord);
			
			vec4 uColorAB = uColorA - uColorB;
			outColor = uColorB + step(0.4, space.x) * uColorAB;
			outColor.w = object.w;
		}
	)GLSL" };

	const char* vertex_shader_screen_rect{ R"GLSL(
		#version 450 core
		precision highp float;
		out vec2 vTexCoord;
		const vec2 pos[4] = vec2[](
		    vec2(-1.0, -1.0),
		    vec2( 1.0, -1.0),
		    vec2(-1.0,  1.0),
		    vec2( 1.0,  1.0)
		);
		const vec2 tex[4] = vec2[](
		    vec2(0.0, 0.0),
		    vec2(1.0, 0.0),
		    vec2(0.0, 1.0),
		    vec2(1.0, 1.0)
		);
		void main() {
		    vTexCoord = tex[gl_VertexID];
		    gl_Position = vec4(pos[gl_VertexID], 0.0, 1.0);
		}
	)GLSL" };

	const char* fragment_shader_combine_reflection{ R"GLSL(
		#version 450
		precision highp float;

		uniform sampler2D uSamplerDefault;
		uniform sampler2D uSamplerReflection;

		uniform vec2 uReflectNormal;
		uniform vec2 uReflectCenter;
		uniform float uTime;

		in vec2 vTexCoord;
		out vec4 outColor;

		void main() {
			vec2 to_center = vTexCoord - uReflectCenter;
			float dot_normal = dot(to_center, uReflectNormal);
			if (dot_normal > 0) {
				outColor = texture(uSamplerDefault, vTexCoord);
			} else {
				vec2 reflectedTexCoord = vTexCoord - 2.0 * dot_normal * uReflectNormal;
				// reflectedTexCoord.x += dot_normal * sin(reflectedTexCoord.y * 40.0 + uTime * 3.0) * 0.05;
				vec4 reflectColor = 0.6 * texture(uSamplerReflection, reflectedTexCoord);
				outColor = reflectColor;
			}
		}
	)GLSL" };

	const char* fragment_shader_post_process{ R"GLSL(
		#version 450
		precision highp float;

		uniform sampler2D uSampler;

		in vec4 vColor;
		in vec2 vTexCoord;

		out vec4 outColor;

		void main() {
			// outColor = vec4(vTexCoord, 0.0, 1.0);
			outColor = texture(uSampler, vTexCoord);
		}
	)GLSL" };

	// render trail
	// bbox
	const char* fragment_shader_trail{ R"GLSL(
		#version 450 core
		precision highp float;

		uniform float uColorMask;
		uniform float uShapeType;

		uniform vec2 uCenter;
		uniform vec2 uNormal;
		uniform float uRadius;

		uniform vec2 uCameraOffset;

		const vec2 screenResolution = vec2(1920.0, 1080.0);

		in vec2 vTexCoord;
		out vec4 outColor;

		float sdCircle(vec2 p, float radius)
		{
			float dist = length(p);
			return step(dist, radius);
		}

		float sdLine(vec2 p, vec2 normal)
		{
			float dot_normal = dot(p, normal);
			return step(0.0, dot_normal);
		}
		
		vec2 texToPos(vec2 uv)
		{
			vec2 pos = vec2(uv.x - 0.5, 0.5 - uv.y); // vec2(uv.x, 1.0 - uv.y);
			return vec2(screenResolution.x * pos.x, screenResolution.y * pos.y) + uCameraOffset;
		}

		void main() {
			vec2 p = texToPos(vTexCoord * 1.0);
			vec2 from_center = p - uCenter;

			float maskLine = sdLine(from_center, uNormal);
			float maskCircle = sdCircle(from_center, uRadius);

			float mask = maskLine + step(1.5, uShapeType) * (maskCircle - maskLine);
			outColor = vec4(uColorMask, uColorMask, uColorMask, mask); // TODO: one channel ?
		}
	)GLSL" };
}

namespace foo
{
	void ShaderManager::Initialize()
	{
		// default shader ========
		m_shader_ids[(size_t)ShaderName::DEFAULT] = CompileShaderProgram(vertex_shader_default_batch, fragment_shader_default);
		// tilemap shader
		m_shader_ids[(size_t)ShaderName::TILE_MAP] = CompileShaderProgram(vertex_shader_tile_map, fragment_shader_default);
		// particle shader
		m_shader_ids[(size_t)ShaderName::PARTICLE] = CompileShaderProgram(vertex_shader_particle, fragment_shader_default);
		// space
		m_shader_ids[(size_t)ShaderName::SPACE] = CompileShaderProgram(vertex_shader_space, fragment_shader_space);

		// trail shader
		m_shader_ids[(size_t)ShaderName::TRAIL] = CompileShaderProgram(vertex_shader_screen_rect, fragment_shader_trail);

		// combine reflection shader  ========
		m_shader_ids[(size_t)ShaderName::COMBINE_REFLECTION] = CompileShaderProgram(vertex_shader_screen_rect, fragment_shader_combine_reflection);

		// post processing shader ========
		m_shader_ids[(size_t)ShaderName::POST_PROCESS] = CompileShaderProgram(vertex_shader_screen_rect, fragment_shader_post_process);


		// set default shader projection matrix
		Matrix4x4 projection;
		projection = Matrix4x4::OrthographicRightHanded(SCREEN_WIDTH, -SCREEN_HEIGHT, 0.0f, 1.0f);
		// matrix
		float float_array[Matrix4x4::FLOAT_ARRAY_SIZE];
		projection.ToFloatArray(float_array);
		
		glUseProgram(m_shader_ids[(size_t)ShaderName::DEFAULT]);
		glUniformMatrix4fv(glGetUniformLocation(m_shader_ids[(size_t)ShaderName::DEFAULT], "uProjection"), 1, GL_FALSE, float_array);
		glUseProgram(m_shader_ids[(size_t)ShaderName::TILE_MAP]);
		glUniformMatrix4fv(glGetUniformLocation(m_shader_ids[(size_t)ShaderName::TILE_MAP], "uProjection"), 1, GL_FALSE, float_array);
		glUseProgram(m_shader_ids[(size_t)ShaderName::PARTICLE]);
		glUniformMatrix4fv(glGetUniformLocation(m_shader_ids[(size_t)ShaderName::PARTICLE], "uProjection"), 1, GL_FALSE, float_array);
		glUseProgram(m_shader_ids[(size_t)ShaderName::SPACE]);
		glUniformMatrix4fv(glGetUniformLocation(m_shader_ids[(size_t)ShaderName::SPACE], "uProjection"), 1, GL_FALSE, float_array);

	}
	void ShaderManager::Finalize()
	{
		for (int i = 0; i < static_cast<int>(ShaderName::MAX); i++)
		{
			glDeleteProgram(m_shader_ids[i]);
		}
	}

	unsigned int ShaderManager::GetShaderProgramId() const
	{
		return m_shader_current;
	}

	unsigned int ShaderManager::GetShaderProgramId(ShaderName name) const
	{
		return m_shader_ids[(size_t)name];
	}

	void ShaderManager::SetShader(ShaderName name)
	{
		unsigned int id = m_shader_ids[(size_t)name];
		glUseProgram(id);
		m_shader_current = id;
	}

	// utils ========
	unsigned int ShaderManager::CompileShaderProgram(const char* vert, const char* frag)
	{
		GLint result;
		GLchar shaderLog[1024];
		GLsizei shaderLogSize;

		GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);

		FOO_ASSERT(vertexShaderId != 0, "Failed to create vertex shader\n");
		glShaderSource(vertexShaderId, 1, &vert, 0);
		glCompileShader(vertexShaderId);
		glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &result);
		if (!result)
		{
			glGetShaderInfoLog(vertexShaderId, sizeof(shaderLog), &shaderLogSize, shaderLog);
			FOO_ASSERT(false, "Failed to compile vertex shader: %s\n", shaderLog);
		}

		GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
		FOO_ASSERT(fragmentShaderId != 0, "Failed to create fragment shader\n");
		glShaderSource(fragmentShaderId, 1, &frag, 0);
		glCompileShader(fragmentShaderId);
		glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &result);
		if (!result)
		{
			glGetShaderInfoLog(fragmentShaderId, sizeof(shaderLog), &shaderLogSize, shaderLog);
			FOO_ASSERT(false, "Failed to compile fragment shader: %s\n", shaderLog);
		}

		GLuint shaderProgramId = glCreateProgram();
		FOO_ASSERT(shaderProgramId != 0, "Failed to create shader program\n");
		glAttachShader(shaderProgramId, vertexShaderId);
		glAttachShader(shaderProgramId, fragmentShaderId);
		glLinkProgram(shaderProgramId);
		glUseProgram(shaderProgramId);

		// delete vertex and fragment shader
		glDetachShader(shaderProgramId, vertexShaderId);
		glDetachShader(shaderProgramId, fragmentShaderId);

		glDeleteShader(vertexShaderId);
		glDeleteShader(fragmentShaderId);

		// reset shader state
		glUseProgram(0);

		return shaderProgramId;
	}

	// set uniform variables ========
	void ShaderManager::SetTransformMatrix(const Matrix4x4& transform) const
	{
		float float_array[Matrix4x4::FLOAT_ARRAY_SIZE];
		transform.ToFloatArray(float_array);
		// current shader
		glUniformMatrix4fv(glGetUniformLocation(m_shader_current, "uTransform"), 1, GL_FALSE, float_array);
	}
	void ShaderManager::SetInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(m_shader_current, name.c_str()), value);
	}
	void ShaderManager::SetFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(m_shader_current, name.c_str()), value);
	}
	void ShaderManager::SetVec2(const std::string& name, const Vector2& value) const
	{
		glUniform2f(glGetUniformLocation(m_shader_current, name.c_str()), value.x, value.y);
	}

	void ShaderManager::SetVec4(const std::string& name, const Vector4& value) const
	{
		glUniform4f(glGetUniformLocation(m_shader_current, name.c_str()), value.x, value.y, value.z, value.w);
	}

	void ShaderManager::SetTexture(const std::string& sampler_name, unsigned int texture_id, int sampler_value) const
	{
		glActiveTexture(GL_TEXTURE0 + sampler_value);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		glUniform1i(glGetUniformLocation(m_shader_current, sampler_name.c_str()), sampler_value);
	}
}