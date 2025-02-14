#pragma once

#ifdef BS_NETWORKING_ON

#include "Core.h"

namespace Bloodshot::Networking::Private
{
	class FNetworkingSystem final
	{
		friend class ::Bloodshot::Private::IEngineContext;
		friend class ::Bloodshot::Private::FEngineEditorContext;
		friend class ::Bloodshot::Private::FEngineGameContext;

		static void Execute(float DeltaTime);
	};
}

#endif