#include "OpenGL/OpenGLShader.h"
#include "OpenGL/OpenGLHeader.h"

namespace Bloodshot
{
	static GLuint CreateShader(const GLenum Type, FStringView Src)
	{
		const char* SrcRaw = Src.GetData();
		const GLuint ShaderID = glCreateShader(Type);
		glShaderSource(ShaderID, 1, &SrcRaw, nullptr);
		glCompileShader(ShaderID);
		GLint CompileResult;
		glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &CompileResult);
		if (!CompileResult)
		{
			GLint LogLength = 0;
			glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &LogLength);
			GLchar* const LogBuffer = reinterpret_cast<GLchar*>(FMemory::Allocate(LogLength, EAllocationType::Temporary));
			glGetProgramInfoLog(ShaderID, LogLength, nullptr, LogBuffer);
			BS_LOG(Fatal, "{} shader compile error: {}", OpenGLShaderTypeToString(Type), (const char*)LogBuffer);
		}

		return ShaderID;
	}

	FOpenGLShader::FOpenGLShader(FStringView Name, FStringView VertexShaderSrc, FStringView FragmentShaderSrc)
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
			GLint LogLength = 0;
			glGetShaderiv(UniqueID, GL_INFO_LOG_LENGTH, &LogLength);
			GLchar* const LogBuffer = reinterpret_cast<GLchar*>(FMemory::Allocate(LogLength, EAllocationType::Temporary));
			glGetProgramInfoLog(UniqueID, LogLength, nullptr, LogBuffer);
			BS_LOG(Fatal, "{} shader link error: {}", Name, (const char*)LogBuffer);
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
		glUniformMatrix4fv(glGetUniformLocation(UniqueID, Name), 1, GL_FALSE, glm::value_ptr(Value));
		glUseProgram(0);
	}
}
