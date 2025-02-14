#pragma once

#include "Platform/Platform.h"

namespace Bloodshot
{
	// BSTODO: Temp

	struct FApplication final
	{
		friend class Private::IEngineContext;
		friend class Private::FEngineEditorContext;
		friend class Private::FEngineGameContext;

	public:
		static FApplication& GetInstance();

		NODISCARD FORCEINLINE bool IsExitRequested() const noexcept
		{
			return bExitRequested;
		}

		NODISCARD static void RequestExit()
		{
			GetInstance().bExitRequested = true;
		}

	private:
		FApplication() {}

		bool bExitRequested = false;
	};
}
