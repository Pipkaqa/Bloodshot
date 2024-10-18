#pragma once

#ifdef BS_NETWORKING_ON
#include "Networking.h"

#include <cstdint>

namespace Bloodshot::Networking
{
	struct FNetPacketTransform final : public INetPacket
	{
		int x = 0;
		int y = 0;
		int z = 0;
	};

	enum class ENetPacketTypeDefault : uint16_t
	{
		Transform = 0,
	};
}
#endif
