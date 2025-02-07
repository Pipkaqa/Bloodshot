#pragma once

#include "Platform/Platform.h"
#include "Templates/Singleton.h"

namespace Bloodshot
{
	class FEngineState final : public TManager<FEngineState>
	{
		friend class IEngineContext;
		friend class FEngineEditorContext;
		friend class FEngineGameContext;

	public:
		NODISCARD static bool IsRunning();
		NODISCARD static bool IsSimulating();

	private:
		FEngineState();

		bool bRunning = false;
		bool bSimulating = false;

		virtual void Init() override;
		virtual void Dispose() override;
	};
}
