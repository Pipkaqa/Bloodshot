#pragma once

#include "Texture.h"

#include <cstdint>
#include <string_view>

namespace Bloodshot
{
	class FOpenGLTexture final : public ITexture
	{
	public:
		FOpenGLTexture(const uint32_t Width,
			const uint32_t Height,
			const EColorChannels Channels,
			const void* Data);
		virtual ~FOpenGLTexture() override;

		virtual void Bind(const ETextureUnit Unit) const override;
		virtual void Unbind(const ETextureUnit Unit) const override;
	};
}
