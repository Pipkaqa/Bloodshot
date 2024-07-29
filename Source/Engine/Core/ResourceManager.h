#pragma once

#include "Rendering/Shader.h"
#include "Utility/ISingleton.h"

namespace Bloodshot
{
	class ResourceManager final : public ISingleton<ResourceManager>
	{
		CORE_MODULE;

	public:
		struct Config final
		{
			const char* m_ResourcesPath = "Resources";
			const char* m_SpritesExtension = ".png";
		};

		NODISCARD FORCEINLINE static const Config& GetConfig()
		{
			return s_Instance->m_Config;
		}

		static Shader* LoadShader(const std::string_view name,
			const std::string_view vertexShaderSource,
			const std::string_view fragmentShaderSource,
			const bool force);

	private:
		using ISingleton::Create;

		Config m_Config = {};
		std::vector<std::pair<std::string, UniquePointer<Shader>>> m_Shaders;

		NODISCARD static ResourceManager* Create(const Config& config);

		void Init() override;
		void Dispose() override;
	};
}