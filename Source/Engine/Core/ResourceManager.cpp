#include "ResourceManager.h"

#include "Debug/Logger.h"

namespace Bloodshot
{
	ResourceManager* ResourceManager::Create(const Config& config)
	{
		ISingleton::Create();

		FL_CORE_DEBUG("Creating resource manager...");

		s_Instance->m_Config = &config;

		return s_Instance;
	}

	//Image ResourceManager::LoadImage(const std::string_view filename)
	//{
	//	const auto& config = s_Instance->m_Config;
	//
	//	const auto& fullPath = std::format("{0}{1}{2}", config.m_ResourcesPath, filename.data(), config.m_SpritesExtension);
	//
	//	FL_CORE_TRACE("Loading image [{0}]...", fullPath);
	//
	//	auto& images = s_Instance->m_Images;
	//
	//	if (images.contains(fullPath)) return images[fullPath];
	//
	//	Image image = Raylib::LoadImage(fullPath.c_str());
	//
	//	images[fullPath] = image;
	//
	//	return image;
	//}
	//
	//Texture ResourceManager::LoadTexture(const std::string_view filename)
	//{
	//	const auto& config = s_Instance->m_Config;
	//
	//	const auto& fullPath = std::format("{0}{1}{2}", config.m_ResourcesPath, filename.data(), config.m_SpritesExtension);
	//
	//	FL_CORE_TRACE("Loading texture [{0}]...", fullPath);
	//
	//	auto& textures = s_Instance->m_Textures;
	//
	//	if (textures.contains(fullPath)) return textures[fullPath];
	//
	//	Texture texture = Raylib::LoadTexture(fullPath.c_str());
	//
	//	textures[fullPath] = texture;
	//
	//	return texture;
	//}

	void ResourceManager::Dispose()
	{
		FL_CORE_DEBUG("Destroying resource manager...");

		//for (auto& [path, image] : m_Images)
		//{
		//	FL_CORE_TRACE("Unloading image [{0}]...", path);
		//
		//	Raylib::UnloadImage(image);
		//}
		//
		//m_Images.clear();
		//
		//for (auto& [path, texture] : m_Textures)
		//{
		//	FL_CORE_TRACE("Unloading texture [{0}]...", path);
		//
		//	Raylib::UnloadTexture(texture);
		//}
		//
		//m_Textures.clear();
	}
}
