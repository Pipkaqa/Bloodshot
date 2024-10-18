#include "ResourceManager.h"
#include "AssertionMacros.h"
#include "FileIO.h"
#include "Logging/LoggingMacros.h"
#include "OpenGL/OpenGLFrameBuffer.h"
#include "OpenGL/OpenGLIndexBuffer.h"
#include "OpenGL/OpenGLShader.h"
#include "OpenGL/OpenGLUniformBuffer.h"
#include "OpenGL/OpenGLVertexArray.h"
#include "OpenGL/OpenGLVertexBuffer.h"
#include "Profiling/ProfilingMacros.h"
#include "Renderer.h"
#include "Vertex.h"

namespace Bloodshot
{
	FResourceManager::FResourceManager()
	{
		BS_ASSERT(!Instance, "Attempting to create second ResourceManager");

		Instance = this;
	}

	void FResourceManager::Init()
	{
		BS_LOG(Debug, "Creating ResourceManager...");
	}

	void FResourceManager::Dispose()
	{
		BS_LOG(Debug, "Destroying ResourceManager...");

		VertexArrayList.clear();
		VertexBufferList.clear();
		IndexBufferList.clear();
		UniformBufferList.clear();
		FrameBufferList.clear();
		ShadersTable.clear();
	}

	IVertexArray* FResourceManager::CreateVertexArray()
	{
		std::list<TUniquePtr<IVertexArray>>& VertexArrayList = Instance->VertexArrayList;

		IVertexArray* VertexArray = nullptr;

		switch (IRenderer::GetType())
		{
			case ERendererType::OpenGL:
			{
				VertexArray = new FOpenGLVertexArray();
				break;
			}
			case ERendererType::Vulkan:
			{
				// BSTODO: Add Vulkan support
				BS_LOG(Fatal, "Vulkan is not supported");
				break;
			}
		}

		VertexArrayList.emplace_back(MakeUnique(VertexArray));
		return VertexArray;
	}

	IVertexBuffer* FResourceManager::CreateVertexBuffer(const std::vector<FVertex>& Vertices)
	{
		std::list<TUniquePtr<IVertexBuffer>>& VertexBufferList = Instance->VertexBufferList;

		IVertexBuffer* VertexBuffer = nullptr;

		switch (IRenderer::GetType())
		{
			case ERendererType::OpenGL:
			{
				const uint32_t VertexCount = (uint32_t)Vertices.size();

				VertexBuffer = new FOpenGLVertexBuffer(Vertices.data(), VertexCount * sizeof(FVertex), VertexCount);
				break;
			}
			case ERendererType::Vulkan:
			{
				// BSTODO: Add Vulkan support
				BS_LOG(Fatal, "Vulkan is not supported");
				break;
			}
		}

		VertexBufferList.emplace_back(MakeUnique(VertexBuffer));
		return VertexBuffer;
	}

	IIndexBuffer* FResourceManager::CreateIndexBuffer(const std::vector<uint32_t>& Indices)
	{
		std::list<TUniquePtr<IIndexBuffer>>& IndexBufferList = Instance->IndexBufferList;

		IIndexBuffer* IndexBuffer = nullptr;

		switch (IRenderer::GetType())
		{
			case ERendererType::OpenGL:
			{
				IndexBuffer = new FOpenGLIndexBuffer(Indices.size(), Indices.data());
				break;
			}
			case ERendererType::Vulkan:
			{
				// BSTODO: Add Vulkan support
				BS_LOG(Fatal, "Vulkan is not supported");
				break;
			}
		}

		IndexBufferList.emplace_back(MakeUnique(IndexBuffer));
		return IndexBuffer;
	}

	IUniformBuffer* FResourceManager::CreateUniformBuffer(const uint32_t Size, const uint32_t Binding)
	{
		std::list<TUniquePtr<IUniformBuffer>>& UniformBufferList = Instance->UniformBufferList;

		IUniformBuffer* UniformBuffer = nullptr;

		switch (IRenderer::GetType())
		{
			case ERendererType::OpenGL:
			{
				UniformBuffer = new FOpenGLUniformBuffer(Size, Binding);
				break;
			}
			case ERendererType::Vulkan:
			{
				// BSTODO: Add Vulkan support
				BS_LOG(Fatal, "Vulkan is not supported");
				break;
			}
		}

		UniformBufferList.emplace_back(MakeUnique(UniformBuffer));
		return UniformBuffer;
	}

	IFrameBuffer* FResourceManager::CreateFrameBuffer(const FFrameBufferSettings& Settings)
	{
		std::list<TUniquePtr<IFrameBuffer>>& FrameBufferList = Instance->FrameBufferList;

		IFrameBuffer* FrameBuffer = nullptr;

		switch (IRenderer::GetType())
		{
			case ERendererType::OpenGL:
			{
				FrameBuffer = new FOpenGLFrameBuffer(Settings);
				break;
			}
			case ERendererType::Vulkan:
			{
				// BSTODO: Add Vulkan support
				BS_LOG(Fatal, "Vulkan is not supported");
				break;
			}
		}

		FrameBufferList.emplace_back(MakeUnique(FrameBuffer));
		return FrameBuffer;
	}

	IShader* FResourceManager::CreateShader(std::string_view Name,
		std::string_view VertexShaderSrc,
		std::string_view FragmentShaderSrc,
		const bool bReloadIfExists)
	{
		BS_PROFILE_FUNCTION();

		BS_LOG(Trace, "Loading Shader: {0}...", Name);

		FNameShaderTable& ShadersTable = Instance->ShadersTable;

		const std::string& NameString = std::string(Name);

		FNameShaderTable::const_iterator ShaderIt = ShadersTable.find(NameString);

		if (ShaderIt != ShadersTable.end())
		{
			if (!bReloadIfExists)
			{
				return ShaderIt->second.get();
			}
		}

		IShader* Shader = nullptr;

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

		ShadersTable.emplace(std::make_pair(NameString, MakeUnique(Shader)));

		return Shader;
	}

	IShader* FResourceManager::LoadShader(std::string_view Name,
		std::string_view VertexShaderPath,
		std::string_view FragmentShaderPath,
		const bool bReloadIfExists)
	{
		return CreateShader(Name, IFileIO::ReadFile(VertexShaderPath), IFileIO::ReadFile(FragmentShaderPath), bReloadIfExists);
	}
}
