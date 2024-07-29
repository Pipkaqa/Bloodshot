#pragma once

#include "Core/EngineFramework.h"
#include "Math/Math.h"

namespace Bloodshot
{
	class Shader abstract
	{
		RESOURCE;

	public:
		Shader(std::string_view name);
		virtual ~Shader() {}

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void SetUniformInt(const char* name, const int value) = 0;
		virtual void SetUniformFloat(const char* name, const float value) = 0;
		virtual void SetUniformMat4(const char* name, const glm::mat4& value) = 0;

	protected:
		std::string m_Name = "Unknown";
	};
}
