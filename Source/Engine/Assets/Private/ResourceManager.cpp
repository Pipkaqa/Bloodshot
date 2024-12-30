#include "ResourceManager.h"
#include "AssertionMacros.h"
#include "FileIO.h"
#include "Logging/LoggingMacros.h"
#include "OpenGL/OpenGLFrameBuffer.h"
#include "OpenGL/OpenGLIndexBuffer.h"
#include "OpenGL/OpenGLShader.h"
#include "OpenGL/OpenGLTexture.h"
#include "OpenGL/OpenGLUniformBuffer.h"
#include "OpenGL/OpenGLVertexArray.h"
#include "OpenGL/OpenGLVertexBuffer.h"
#include "Profiling/ProfilingMacros.h"
#include "Renderer.h"
#include "StbImage.h"
#include "Texture.h"
#include "Vertex.h"
#include "VertexArray.h"

namespace Bloodshot
{
	FResourceManager::FResourceManager()
	{
		Instance = this;
	}

	void FResourceManager::Init()
	{
		BS_LOG(Debug, "Creating ResourceManager...");
	}

	void FResourceManager::Dispose()
	{
		BS_LOG(Debug, "Destroying ResourceManager...");

		ShadersTable.clear();
	}

	TUniquePtr<IVertexArray> FResourceManager::CreateVertexArray()
	{
		switch (IRenderer::GetType())
		{
			case ERendererType::OpenGL:
			{
				return MakeUnique<FOpenGLVertexArray>();
			}
			case ERendererType::Vulkan:
			{
				// BSTODO: Add Vulkan support
				BS_LOG(Fatal, "Vulkan is not supported");
				break;
			}
		}

		return nullptr;
	}

	TUniquePtr<IVertexBuffer> FResourceManager::CreateVertexBuffer(const std::vector<FVertex>& Vertices)
	{
		switch (IRenderer::GetType())
		{
			case ERendererType::OpenGL:
			{
				const uint32_t VertexCount = (uint32_t)Vertices.size();

				return MakeUnique<FOpenGLVertexBuffer>(Vertices.data(),
					VertexCount * sizeof(FVertex),
					VertexCount);
			}
			case ERendererType::Vulkan:
			{
				// BSTODO: Add Vulkan support
				BS_LOG(Fatal, "Vulkan is not supported");
				break;
			}
		}

		return nullptr;
	}

	TUniquePtr<IIndexBuffer> FResourceManager::CreateIndexBuffer(const std::vector<uint32_t>& Indices)
	{
		switch (IRenderer::GetType())
		{
			case ERendererType::OpenGL:
			{
				return MakeUnique<FOpenGLIndexBuffer>((uint32_t)Indices.size(), Indices.data());
			}
			case ERendererType::Vulkan:
			{
				// BSTODO: Add Vulkan support
				BS_LOG(Fatal, "Vulkan is not supported");
				break;
			}
		}

		return nullptr;
	}

	TUniquePtr<IUniformBuffer> FResourceManager::CreateUniformBuffer(const uint32_t Size, const uint32_t Binding)
	{
		switch (IRenderer::GetType())
		{
			case ERendererType::OpenGL:
			{
				return MakeUnique<FOpenGLUniformBuffer>(Size, Binding);
			}
			case ERendererType::Vulkan:
			{
				// BSTODO: Add Vulkan support
				BS_LOG(Fatal, "Vulkan is not supported");
				break;
			}
		}

		return nullptr;
	}

	TUniquePtr<IFrameBuffer> FResourceManager::CreateFrameBuffer(const FFrameBufferSettings& Settings)
	{
		switch (IRenderer::GetType())
		{
			case ERendererType::OpenGL:
			{
				return MakeUnique<FOpenGLFrameBuffer>(Settings);
				break;
			}
			case ERendererType::Vulkan:
			{
				// BSTODO: Add Vulkan support
				BS_LOG(Fatal, "Vulkan is not supported");
				break;
			}
		}

		return nullptr;
	}

	TUniquePtr<IShader> FResourceManager::CreateShader(std::string_view Name,
		std::string_view VertexShaderSrc,
		std::string_view FragmentShaderSrc)
	{
		BS_PROFILE_FUNCTION();

		BS_LOG(Trace, "Creating Shader: {0}...", Name);

		FNameShaderTable& ShadersTable = Instance->ShadersTable;

		const std::string& NameString = std::string(Name);

		FNameShaderTable::const_iterator ShaderIt = ShadersTable.find(NameString);

		TReference<IShader> Shader = nullptr;

		switch (IRenderer::GetType())
		{
			case ERendererType::OpenGL:
			{
				Shader = new FOpenGLShader(Name, VertexShaderSrc, FragmentShaderSrc);
				break;
			}
			case ERendererType::Vulkan:
			{
				// BSTODO: Add Vulkan support
				BS_LOG(Fatal, "Vulkan is not supported");
				break;
			}
		}

		// BSTODO: #AFTER_EDITOR, Write error in editor console
		if (!Shader)
		{
			BS_LOG(Error, "Failed to load Shader");
			return nullptr;
		}

		ShadersTable.emplace(std::make_pair(NameString, Shader));

		return MakeUnique<IShader>(Shader);
	}

	TUniquePtr<IShader> FResourceManager::CreateShaderFromFile(std::string_view Name,
		std::string_view VertexShaderPath,
		std::string_view FragmentShaderPath)
	{
		return CreateShader(Name, IFileIO::ReadFile(VertexShaderPath), IFileIO::ReadFile(FragmentShaderPath));
	}

	TUniquePtr<ITexture> FResourceManager::CreateTexture(std::string_view Path, const bool bFlipped)
	{
		stbi_set_flip_vertically_on_load(bFlipped);

		int Width;
		int Height;
		int Channels;

		uint8_t* const Data = stbi_load(Path.data(),
			&Width,
			&Height,
			&Channels,
			STBI_rgb_alpha);

		if (!Data)
		{
			BS_LOG(Error, "Failed to load Texture: {0}", Path.empty() ? "???" : Path);
			return nullptr;
		}

		ITexture* Texture = nullptr;

		switch (IRenderer::GetType())
		{
			case ERendererType::OpenGL:
			{
				Texture = new FOpenGLTexture(Width, Height, StaticCast<EColorChannels>(Channels), Data);
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

		return MakeUnique<ITexture>(Texture);
	}

	TUniquePtr<ITexture> FResourceManager::CreateTextureFromMemory(const void* const Data, const uint32_t DataSize)
	{
		int Width;
		int Height;
		int Channels;

		void* const LoadedData = stbi_load_from_memory((const stbi_uc*)Data, DataSize, &Width, &Height, &Channels, 0);

		if (!LoadedData)
		{
			BS_LOG(Error, "Failed to load Texture from memory");
			return nullptr;
		}

		ITexture* Texture = nullptr;

		switch (IRenderer::GetType())
		{
			case ERendererType::OpenGL:
			{
				Texture = new FOpenGLTexture(Width, Height, StaticCast<EColorChannels>(Channels), LoadedData);
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

		return MakeUnique<ITexture>(Texture);
	}
}
