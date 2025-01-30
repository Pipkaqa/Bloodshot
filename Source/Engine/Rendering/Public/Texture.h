#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace Bloodshot
{
	enum class EColorChannels : uint8_t
	{
		R = 0,
		RG,
		RGB,
		RGBA
	};

	enum class ETextureUnit : uint8_t
	{
		Color = 0,
		Shadow = 1,
		Normal = 2,
		Random = 3,
		Displacement = 4,
		Albedo = 5,
		Roughness = 6,
		Motion = 7,
		Specular = 8,
		CascacdeShadow0 = Shadow,
		CascacdeShadow1 = 9,
		CascacdeShadow2 = 10,
		ShadowCubeMap = 11,
		ShadowMapRandomOffset = 12,
		DetailMap = 13,
		Metallic = 14,
		Height = 15,
	};

	class ITexture
	{
	public:
		ITexture(const uint32_t Width, const uint32_t Height, const EColorChannels Channels);
		virtual ~ITexture() {}

		virtual void Bind(const ETextureUnit Unit) const = 0;
		virtual void Unbind(const ETextureUnit Unit) const = 0;

	protected:
		uint32_t UniqueID = 0;
		uint32_t Width;
		uint32_t Height;
		EColorChannels Channels;
	};
}
