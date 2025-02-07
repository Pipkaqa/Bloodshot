#pragma once

#include "Templates/Singleton.h"

namespace Bloodshot
{
	// BSTODO: Temp

	struct FApplication final : public TSingleton<FApplication>
	{
		friend class IEngineContext;
		friend class FEngineEditorContext;
		friend class FEngineGameContext;

	public:
		static void Close();

	private:
		FApplication();

		bool bShouldClose = false;
	};
}
