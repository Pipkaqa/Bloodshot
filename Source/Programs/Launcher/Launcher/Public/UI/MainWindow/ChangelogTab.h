#pragma once

#include "BaseUI.h"
#include "Platform/Platform.h"

namespace Bloodshot::Launcher
{
	class FChangelogTab final : public IBaseUI
	{
		friend class FMainWindow;

	public:
		NODISCARD static FChangelogTab& GetInstance();

	private:
		FORCEINLINE FChangelogTab()
		{
			Setup();
		}

		virtual void Draw() override;

		void Setup();
	};
}