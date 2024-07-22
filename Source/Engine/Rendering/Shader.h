#pragma once

#include "Math/Math.h"

namespace Bloodshot
{
	class Shader abstract
	{
	public:
		virtual ~Shader() {}

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void SetUniformInt(const char* name, const int value) = 0;
		virtual void SetUniformFloat(const char* name, const float value) = 0;
		virtual void SetUniformMat4(const char* name, const glm::mat4& value) = 0;

	private:
		Shader(const char* name, const char* vertexShaderSource, const char* fragmentShaderSource);

		const char* m_Name = "";
	};
}
