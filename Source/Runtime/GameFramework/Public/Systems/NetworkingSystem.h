#pragma once

#ifdef BS_NETWORKING_ON

#include "Core.h"

namespace Bloodshot::Private::Networking
{
	class FNetworkingSystem final
	{
		friend class ::Bloodshot::Private::Launch::IEngineContext;
		friend class ::Bloodshot::Private::Launch::FEngineEditorContext;
		friend class ::Bloodshot::Private::Launch::FEngineGameContext;

		static void Execute(float DeltaTime);
	};
}

#endif