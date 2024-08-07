#pragma once

#include "Shader.h"

namespace Bloodshot
{
	class OpenGLShader : public SShader
	{
	public:
		OpenGLShader(std::string_view Name, std::string_view VertexShaderSrc, std::string_view FragmentShaderSrc);
		virtual ~OpenGLShader() override;

		virtual void Bind() const noexcept override;
		virtual void Unbind() const noexcept override;

		virtual void SetUniformInt(const char* Name, const int Value) noexcept override;
		virtual void SetUniformFloat(const char* Name, const float Value) noexcept override;
		virtual void SetUniformMat4(const char* Name, const glm::mat4& Value) noexcept override;

	private:
		uint32_t UniqueID = 0;
	};
}
