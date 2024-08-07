#include "ResourceManager.h"

#include "AssertionMacros.h"
#include "FileIO.h"
#include "Logging/LoggingMacros.h"
#include "OpenGL/OpenGLShader.h"
#include "Profiling/ProfilingMacros.h"
#include "Renderer.h"

namespace Bloodshot
{
	FResourceManager::FResourceManager()
	{
		BS_ASSERT(!Instance, "Attempting to create second ResourceManager");

		Instance = this;
	}

	SShader* FResourceManager::LoadShader(std::string_view Name,
		std::string_view VertexShaderPath,
		std::string_view FragmentShaderPath,
		const bool bReloadIfExists)
	{
		BS_PROFILE_FUNCTION();

		BS_LOG(Trace, "Loading Shader: {0}...", Name);

		FNameShaderTable& Shaders = Instance->ShadersTable;

		for (FNameShaderTable::const_iterator It = Shaders.begin(); It != Shaders.end(); ++It)
		{
			if (It->first != Name) continue;

			if (!bReloadIfExists) return It->second.get();

			Shaders.erase(It);

			break;
		}

		SShader* Shader = nullptr;

		switch (IRenderer::GetType())
		{
			case ERendererType::OpenGL:
			{
				Shader = new OpenGLShader(Name, IFileIO::ReadFile(VertexShaderPath), IFileIO::ReadFile(FragmentShaderPath));
				break;
			}
			case ERendererType::Vulkan:
			{
				// BSTODO: Add Vulkan support
				BS_LOG(Fatal, "Vulkan is not supported");
			}
		}

		// BSTODO: #AFTER_EDITOR, Write error in editor console

		// BSNOTE: Check -> Error (06.08.2024)
		//BS_CHECK(Shader, "Failed to load shader");
		if (!Shader)
		{
			BS_LOG(Error, "Failed to load Shader");
			return nullptr;
		}

		Shaders.emplace_back(std::make_pair(std::string(Name), MakeUnique(Shader)));

		return Shader;
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
}
