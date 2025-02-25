#pragma once

#include "Platform/Platform.h"

namespace Bloodshot
{
	struct FApplication final
	{
		friend class Private::Launch::IEngineContext;
		friend class Private::Launch::FEngineEditorContext;
		friend class Private::Launch::FEngineGameContext;

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
