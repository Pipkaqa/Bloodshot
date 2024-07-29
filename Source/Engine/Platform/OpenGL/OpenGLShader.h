#pragma once

#include "Rendering/Shader.h"

namespace Bloodshot
{
	class OpenGLShader : public Shader
	{
		RESOURCE;

		OpenGLShader(const std::string_view name, const std::string_view vertexShaderSource, const std::string_view fragmentShaderSource);
		~OpenGLShader() override;

		uint32_t m_UniqueID = 0;

		void Bind() override;
		void Unbind() override;

		void SetUniformInt(const char* name, const int value) override;
		void SetUniformFloat(const char* name, const float value) override;
		void SetUniformMat4(const char* name, const glm::mat4& value) override;
	};
}
