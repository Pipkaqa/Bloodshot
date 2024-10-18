#include "OpenGL/OpenGLShader.h"
#include "Logging/LoggingMacros.h"
#include "OpenGL/OpenGLHeader.h"

namespace Bloodshot
{
	static GLuint CreateShader(const GLenum Type, std::string_view Src)
	{
		const char* SrcRaw = Src.data();

		const GLuint ShaderID = glCreateShader(Type);
		glShaderSource(ShaderID, 1, &SrcRaw, nullptr);
		glCompileShader(ShaderID);

		GLint CompileResult;
		glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &CompileResult);

		if (!CompileResult)
		{
			// BSTODO: Protect from overflow without using new
			constexpr unsigned LogLength = 1024;
			GLchar Log[LogLength];

			glGetProgramInfoLog(ShaderID, LogLength, nullptr, Log);
			BS_LOG(Fatal, "{0} Shader compile error: {1}", OpenGLShaderTypeToString(Type), Log);
		}

		return ShaderID;
	}

	FOpenGLShader::FOpenGLShader(std::string_view Name, std::string_view VertexShaderSrc, std::string_view FragmentShaderSrc)
		: IShader(Name)
	{
		const GLuint VertexShaderID = CreateShader(GL_VERTEX_SHADER, VertexShaderSrc);
		const GLuint FragmentShaderID = CreateShader(GL_FRAGMENT_SHADER, FragmentShaderSrc);

		UniqueID = glCreateProgram();
		glAttachShader(UniqueID, VertexShaderID);
		glAttachShader(UniqueID, FragmentShaderID);
		glLinkProgram(UniqueID);

		GLint LinkResult;
		glGetProgramiv(UniqueID, GL_LINK_STATUS, &LinkResult);

		if (!LinkResult)
		{
			// BSTODO: Protect from overflow without using new
			constexpr unsigned LogLength = 1024;
			GLchar Log[LogLength];

			glGetProgramInfoLog(UniqueID, LogLength, nullptr, Log);
			BS_LOG(Fatal, "{0} Shader link error: {1}", Name, Log);
		}

		glDeleteShader(VertexShaderID);
		glDeleteShader(FragmentShaderID);
	}

	FOpenGLShader::~FOpenGLShader()
	{
		glDeleteProgram(UniqueID);
	}

	void FOpenGLShader::Bind() const noexcept
	{
		glUseProgram(UniqueID);
	}

	void FOpenGLShader::Unbind() const noexcept
	{
		glUseProgram(0);
	}

	void FOpenGLShader::SetUniformInt(const char* Name, const int Value) noexcept
	{
		glUseProgram(UniqueID);
		glUniform1i(glGetUniformLocation(UniqueID, Name), Value);
		glUseProgram(0);
	}

	void FOpenGLShader::SetUniformFloat(const char* Name, const float Value) noexcept
	{
		glUseProgram(UniqueID);
		glUniform1f(glGetUniformLocation(UniqueID, Name), Value);
		glUseProgram(0);
	}

	void FOpenGLShader::SetUniformMat4(const char* Name, const glm::mat4& Value) noexcept
	{
		glUseProgram(UniqueID);
		glUniformMatrix4fv(glGetUniformLocation(UniqueID, Name), 0, GL_FALSE, glm::value_ptr(Value));
		glUseProgram(0);
	}
}
