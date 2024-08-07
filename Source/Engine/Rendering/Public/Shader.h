#pragma once

#include "MathLibrary.h"

namespace Bloodshot
{
	class SShader abstract
	{
	public:
		SShader(std::string_view Name);
		virtual ~SShader() {}

		virtual void Bind() const noexcept = 0;
		virtual void Unbind() const noexcept = 0;

		virtual void SetUniformInt(const char* Name, const int Value) noexcept = 0;
		virtual void SetUniformFloat(const char* Name, const float Value) noexcept = 0;
		virtual void SetUniformMat4(const char* Name, const glm::mat4& Value) noexcept = 0;

	protected:
		std::string Name;
	};
}
