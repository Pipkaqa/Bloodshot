#include "Texture.h"

namespace Bloodshot
{
	ITexture::ITexture(const uint32_t Width, const uint32_t Height, const EColorChannels Channels)
		: Width(Width)
		, Height(Height)
		, Channels(Channels)
	{
	}
}