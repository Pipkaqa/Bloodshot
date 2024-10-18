#pragma once

#ifdef BS_NETWORKING_ON
#include "Networking.h"
#include "System.h"

namespace Bloodshot::Networking::Private
{
	struct FNetworkingSystem final
	{
		void Execute(float DeltaTime);
	};
}
#endif
