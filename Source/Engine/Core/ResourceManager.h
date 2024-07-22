#pragma once

#include "Platform/Platform.h"
#include "Utility/ISingleton.h"
#include "EngineFramework.h"

#include <string_view>
#include <unordered_map>

namespace Bloodshot
{
	class ResourceManager final : public ISingleton<ResourceManager>
	{
		OWNED_BY_CORE;

	public:
		struct Config final
		{
			const char* m_ResourcesPath = "Resources/";

			const char* m_SpritesExtension = ".png";
		};

		NODISCARD FORCEINLINE static const Config& GetConfig()
		{
			return *s_Instance->m_Config;
		}

		//TODO: static Image LoadImage(const std::string_view filename);
		//TODO: static Texture LoadTexture(const std::string_view filename);

	private:
		const Config* m_Config = nullptr;

		//TODO: std::unordered_map<std::string, Image> m_Images;
		//TODO: std::unordered_map<std::string, Texture> m_Textures;

		NODISCARD static ResourceManager* Create(const Config& config);

		void Init() override {}
		void Dispose() override;
	};
}