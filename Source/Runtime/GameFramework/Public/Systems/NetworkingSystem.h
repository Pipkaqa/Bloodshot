#pragma once

#ifdef BS_NETWORKING_ON

namespace Bloodshot::Networking::Private
{
	struct FNetworkingSystem final
	{
		void Execute(float DeltaTime);
	};
}

#endif