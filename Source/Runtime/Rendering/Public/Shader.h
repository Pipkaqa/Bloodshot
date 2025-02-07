#pragma once

#include "Core.h"

namespace Bloodshot
{
	class IShader
	{
	public:
		IShader(FStringView Name) : Name(Name) {}
		virtual ~IShader() {}

		virtual void Bind() const noexcept = 0;
		virtual void Unbind() const noexcept = 0;

		virtual void SetUniformInt(const char* Name, const int Value) noexcept = 0;
		virtual void SetUniformFloat(const char* Name, const float Value) noexcept = 0;
		virtual void SetUniformMat4(const char* Name, const glm::mat4& Value) noexcept = 0;

	protected:
		FString Name;
	};
}
