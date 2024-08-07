#pragma once

#include "Shader.h"
#include "Templates/Singleton.h"
#include "Templates/SmartPointers.h"

namespace Bloodshot
{
	class FResourceManager final : public TSingleton<FResourceManager>
	{
	public:
		using FNameShaderTable = std::vector<std::pair<std::string, TUniquePtr<SShader>>>;

		FResourceManager();

		static inline const char* ResourcesPath = "Resources";
		static inline const char* SpritesExtension = ".png";

		FNameShaderTable ShadersTable;

		virtual void Init() override;
		virtual void Dispose() override;

		static SShader* LoadShader(std::string_view Name,
			std::string_view VertexShaderSource,
			std::string_view FragmentShaderSource,
			const bool bReloadIfExists = false);
	};
}
