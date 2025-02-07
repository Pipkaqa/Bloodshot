#pragma once

#include "Core.h"

#include "Shader.h"

namespace Bloodshot
{
	class FOpenGLShader final : public IShader
	{
	public:
		FOpenGLShader(FStringView Name, FStringView VertexShaderSrc, FStringView FragmentShaderSrc);
		virtual ~FOpenGLShader() override;

		virtual void Bind() const noexcept override;
		virtual void Unbind() const noexcept override;

		virtual void SetUniformInt(const char* Name, const int Value) noexcept override;
		virtual void SetUniformFloat(const char* Name, const float Value) noexcept override;
		virtual void SetUniformMat4(const char* Name, const glm::mat4& Value) noexcept override;

	private:
		uint32_t UniqueID = 0;
	};
}
