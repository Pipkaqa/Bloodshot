#pragma once

#include "Core.h"

#include "Texture.h"

namespace Bloodshot
{
	struct FMaterial final
	{
		glm::vec3 Color = {};
		glm::vec3 AmbientColor = {};
		glm::vec3 DiffuseColor = {};
		glm::vec3 SpecularColor = {};

		float Roughness = 0.0f;

		bool bIsMetal = false;

		TUniquePtr<ITexture> AlbedoTexture = nullptr;
		TUniquePtr<ITexture> RoughnessTexture = nullptr;
		TUniquePtr<ITexture> MetallicTexture = nullptr;
		TUniquePtr<ITexture> NormalMapTexture = nullptr;
		TUniquePtr<ITexture> DiffuseTexture = nullptr;
		TUniquePtr<ITexture> SpecularTexture = nullptr;
	};
}
