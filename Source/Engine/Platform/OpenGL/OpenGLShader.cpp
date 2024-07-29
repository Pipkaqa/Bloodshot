#include "OpenGLShader.h"

#include "Debug/Logger.h"
#include "OpenGLHeader.h"

namespace Bloodshot
{
	static GLuint CreateShader(GLenum type, std::string_view source)
	{
		auto raw = source.data();

		auto shader = glCreateShader(type);
		glShaderSource(shader, 1, &raw, nullptr);
		glCompileShader(shader);

		GLint result;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

		if (!result)
		{
			GLint length;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

			GLchar* log = new GLchar[length + 1];
			glGetShaderInfoLog(shader, length, &length, log);

			BS_FATAL("{0} shader compile error: {1}", OpenGLShaderTypeToString(type), log);
			delete[] log;

			TERMINATE;
		}

		return shader;
	}

	OpenGLShader::OpenGLShader(const std::string_view name, const std::string_view vertexShaderSource, const std::string_view fragmentShaderSource)
		: Shader(name)
	{
		const auto vertexShader = CreateShader(GL_VERTEX_SHADER, vertexShaderSource);
		const auto fragmentShader = CreateShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

		m_UniqueID = glCreateProgram();
		glAttachShader(m_UniqueID, vertexShader);
		glAttachShader(m_UniqueID, fragmentShader);
		glLinkProgram(m_UniqueID);

		GLint result;
		glGetProgramiv(m_UniqueID, GL_LINK_STATUS, &result);

		if (!result)
		{
			GLint length;
			glGetProgramiv(m_UniqueID, GL_INFO_LOG_LENGTH, &length);

			GLchar* log = new GLchar[length + 1];
			glGetProgramInfoLog(m_UniqueID, length, &length, log);

			BS_FATAL("{0} shader link error: {1}", name, log);
			delete[] log;

			TERMINATE;
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_UniqueID);
	}

	void OpenGLShader::Bind()
	{
		glUseProgram(m_UniqueID);
	}

	void OpenGLShader::Unbind()
	{
		glUseProgram(0);
	}

	void OpenGLShader::SetUniformInt(const char* name, const int value)
	{
		glUseProgram(m_UniqueID);
		glUniform1i(glGetUniformLocation(m_UniqueID, name), value);
		glUseProgram(0);
	}

	void OpenGLShader::SetUniformFloat(const char* name, const float value)
	{
		glUseProgram(m_UniqueID);
		glUniform1f(glGetUniformLocation(m_UniqueID, name), value);
		glUseProgram(0);
	}

	void OpenGLShader::SetUniformMat4(const char* name, const glm::mat4& value)
	{
		glUseProgram(m_UniqueID);
		glUniformMatrix4fv(glGetUniformLocation(m_UniqueID, name), 0, GL_FALSE, glm::value_ptr(value));
		glUseProgram(0);
	}
}
