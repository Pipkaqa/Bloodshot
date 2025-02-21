#include "ResourceManager.h"
#include "Misc/AssertionMacros.h"
#include "OpenGL/OpenGLFrameBuffer.h"
#include "OpenGL/OpenGLIndexBuffer.h"
#include "OpenGL/OpenGLShader.h"
#include "OpenGL/OpenGLTexture.h"
#include "OpenGL/OpenGLUniformBuffer.h"
#include "OpenGL/OpenGLVertexArray.h"
#include "OpenGL/OpenGLVertexBuffer.h"
#include "Renderer.h"
#include "StbImage.h"
#include "Texture.h"
#include "VertexArray.h"

#include <fstream>
#include <sstream>

namespace Bloodshot
{
	TUniquePtr<IVertexArray> IResourceManager::CreateVertexArray()
	{
		BS_PROFILE_FUNCTION();

		BS_LOG(Trace, "Creating VertexArray...");

		TUniquePtr<IVertexArray> VertexArray = nullptr;

		switch (IRenderer::GetType())
		{
			case ERendererType::OpenGL:
			{
				VertexArray = TUniquePtr<IVertexArray>(new FOpenGLVertexArray);
				break;
			}
			case ERendererType::Vulkan:
			{
				// BSTODO: Add Vulkan support
				BS_LOG(Fatal, "Vulkan is not supported");
				break;
			}
		}

		return VertexArray;
	}

	TUniquePtr<IVertexBuffer> IResourceManager::CreateVertexBuffer(const TArray<FVertex>& Vertices)
	{
		BS_PROFILE_FUNCTION();

		BS_LOG(Trace, "Creating VertexBuffer...");

		TUniquePtr<IVertexBuffer> VertexBuffer = nullptr;

		const uint32_t VertexCount = (uint32_t)Vertices.GetSize();

		switch (IRenderer::GetType())
		{
			case ERendererType::OpenGL:
			{
				VertexBuffer = TUniquePtr<IVertexBuffer>(new FOpenGLVertexBuffer(Vertices.GetData(),
					VertexCount * sizeof(FVertex),
					VertexCount));

				break;
			}
			case ERendererType::Vulkan:
			{
				// BSTODO: Add Vulkan support
				BS_LOG(Fatal, "Vulkan is not supported");
				break;
			}
		}

		return VertexBuffer;
	}

	TUniquePtr<IIndexBuffer> IResourceManager::CreateIndexBuffer(const TArray<uint32_t>& Indices)
	{
		BS_PROFILE_FUNCTION();

		BS_LOG(Trace, "Creating IndexBuffer...");

		TUniquePtr<IIndexBuffer> IndexBuffer = nullptr;

		switch (IRenderer::GetType())
		{
			case ERendererType::OpenGL:
			{
				IndexBuffer = TUniquePtr<IIndexBuffer>(new FOpenGLIndexBuffer((uint32_t)Indices.GetSize(),
					Indices.GetData()));

				break;
			}
			case ERendererType::Vulkan:
			{
				// BSTODO: Add Vulkan support
				BS_LOG(Fatal, "Vulkan is not supported");
				break;
			}
		}

		return IndexBuffer;
	}

	TUniquePtr<IUniformBuffer> IResourceManager::CreateUniformBuffer(const uint32_t Size, const uint32_t Binding)
	{
		BS_PROFILE_FUNCTION();

		BS_LOG(Trace, "Creating UniformBuffer...");

		TUniquePtr<IUniformBuffer> UniformBuffer = nullptr;

		switch (IRenderer::GetType())
		{
			case ERendererType::OpenGL:
			{
				UniformBuffer = TUniquePtr<IUniformBuffer>(new FOpenGLUniformBuffer(Size, Binding));
				break;
			}
			case ERendererType::Vulkan:
			{
				// BSTODO: Add Vulkan support
				BS_LOG(Fatal, "Vulkan is not supported");
				break;
			}
		}

		return UniformBuffer;
	}

	TUniquePtr<IFrameBuffer> IResourceManager::CreateFrameBuffer(const FFrameBufferSettings& Settings)
	{
		BS_PROFILE_FUNCTION();

		BS_LOG(Trace, "Creating FrameBuffer...");

		TUniquePtr<IFrameBuffer> FrameBuffer = nullptr;

		switch (IRenderer::GetType())
		{
			case ERendererType::OpenGL:
			{
				FrameBuffer = TUniquePtr<IFrameBuffer>(new FOpenGLFrameBuffer(Settings));
				break;
			}
			case ERendererType::Vulkan:
			{
				// BSTODO: Add Vulkan support
				BS_LOG(Fatal, "Vulkan is not supported");
				break;
			}
		}

		return FrameBuffer;
	}

	TUniquePtr<IShader> IResourceManager::CreateShader(FStringView Name,
		FStringView VertexShaderSrc,
		FStringView FragmentShaderSrc)
	{
		BS_PROFILE_FUNCTION();

		BS_LOG(Trace, "Creating Shader: {}...", Name);

		TUniquePtr<IShader> Shader = nullptr;

		switch (IRenderer::GetType())
		{
			case ERendererType::OpenGL:
			{
				Shader = TUniquePtr<IShader>(new FOpenGLShader(Name, VertexShaderSrc, FragmentShaderSrc));
				break;
			}
			case ERendererType::Vulkan:
			{
				// BSTODO: Add Vulkan support
				BS_LOG(Fatal, "Vulkan is not supported");
				break;
			}
		}

		if (!Shader)
		{
			BS_LOG(Error, "Failed to load Shader");
			return nullptr;
		}

		return Shader;
	}

	TUniquePtr<IShader> IResourceManager::CreateShaderFromFile(FStringView Name,
		FStringView VertexShaderPath,
		FStringView FragmentShaderPath)
	{
		std::ifstream InputStream;

		InputStream.open(VertexShaderPath.GetData());
		BS_CHECK(InputStream.is_open());
		std::stringstream VertexShaderSource;
		VertexShaderSource << InputStream.rdbuf();
		const FString& VSC = VertexShaderSource.str().c_str();
		InputStream.close();

		InputStream.open(FragmentShaderPath.GetData());
		BS_CHECK(InputStream.is_open());
		std::stringstream FragmentShaderSource;
		FragmentShaderSource << InputStream.rdbuf();
		const FString& FSC = FragmentShaderSource.str().c_str();
		InputStream.close();

		return CreateShader(Name, VSC.GetData(), FSC.GetData());
	}

	TUniquePtr<ITexture> IResourceManager::CreateTexture(FStringView Path, const bool bFlipped)
	{
		BS_PROFILE_FUNCTION();

		BS_LOG(Trace, "Creating Texture: {}...", Path);

		stbi_set_flip_vertically_on_load(bFlipped);

		int Width;
		int Height;
		int Channels;

		uint8_t* const Data = stbi_load(Path.GetData(),
			&Width,
			&Height,
			&Channels,
			STBI_rgb_alpha);

		if (!Data)
		{
			BS_LOG(Error, "Failed to load Texture: {}", Path.IsEmpty() ? "???" : Path);
			return nullptr;
		}

		TUniquePtr<ITexture> Texture = nullptr;

		switch (IRenderer::GetType())
		{
			case ERendererType::OpenGL:
			{
				Texture = TUniquePtr<ITexture>(new FOpenGLTexture(Width, Height, StaticCast<EColorChannels>(Channels), Data));
				break;
			}
			case ERendererType::Vulkan:
			{
				// BSTODO: Add Vulkan support
				BS_LOG(Fatal, "Vulkan is not supported");
				break;
			}
		}

		stbi_image_free(Data);

		stbi_set_flip_vertically_on_load(false);

		return Texture;
	}

	TUniquePtr<ITexture> IResourceManager::CreateTextureFromMemory(const void* const Data, const uint32_t DataSize)
	{
		BS_PROFILE_FUNCTION();

		BS_LOG(Trace, "Creating Texture from memory...");

		int Width;
		int Height;
		int Channels;

		void* const LoadedData = stbi_load_from_memory((const stbi_uc*)Data, DataSize, &Width, &Height, &Channels, 0);

		if (!LoadedData)
		{
			BS_LOG(Error, "Failed to load Texture from memory");
			return nullptr;
		}

		TUniquePtr<ITexture> Texture = nullptr;

		switch (IRenderer::GetType())
		{
			case ERendererType::OpenGL:
			{
				Texture = TUniquePtr<ITexture>(new FOpenGLTexture(Width, Height, StaticCast<EColorChannels>(Channels), LoadedData));
				break;
			}
			case ERendererType::Vulkan:
			{
				// BSTODO: Add Vulkan support
				BS_LOG(Fatal, "Vulkan is not supported");
				break;
			}
		}

		stbi_image_free(LoadedData);

		return Texture;
	}
}
