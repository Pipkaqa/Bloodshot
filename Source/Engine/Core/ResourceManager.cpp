#include "ResourceManager.h"

#include "Benchmark/Profiler.h"
#include "Debug/Logger.h"
#include "FileIO.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Rendering/Renderer.h"

namespace Bloodshot
{
	ResourceManager* ResourceManager::Create(const Config& config)
	{
		BS_ASSERT(!s_Instance, "An attempt to create another resource manager");

		s_Instance = new ResourceManager;

		s_Instance->m_Config = config;

		return s_Instance;
	}

	Shader* ResourceManager::LoadShader(const std::string_view name,
		const std::string_view vertexShaderPath,
		const std::string_view fragmentShaderPath,
		const bool force)
	{
		BS_PROFILE_FUNCTION();

		BS_TRACE("Loading shader: [{0}]...", name);

		auto& shaders = s_Instance->m_Shaders;

		auto shaderIt = shaders.begin();

		bool founded = false;

		for (; shaderIt != shaders.end(); ++shaderIt)
		{
			if (shaderIt->first == name)
			{
				founded = true;
				break;
			}
		}

		if (founded)
		{
			if (!force)
			{
				return shaderIt->second.get();
			}

			shaders.erase(shaderIt);
		}

		Shader* shader = nullptr;

		switch (Renderer::GetType())
		{
			case Renderer::Type::OpenGL:
			{
				shader = new OpenGLShader(name, FileIO::ReadFile(vertexShaderPath), FileIO::ReadFile(fragmentShaderPath));
			}

			// BSTODO: Add Vulkan support
		}

		// BSTODO: #AFTER_EDITOR, Write error in editor console
		BS_CHECK_FATAL(shader, "Failed to load shader!");

		shaders.emplace_back(std::string(name), CreateUniquePointer(shader));

		return shader;
	}

	void ResourceManager::Init()
	{
		BS_DEBUG("Creating resource manager...");
	}

	void ResourceManager::Dispose()
	{
		BS_DEBUG("Destroying resource manager...");

		m_Shaders.clear();
	}
}
