#pragma once

#include "BaseUI.h"
#include "Platform/Platform.h"

namespace Bloodshot::Launcher
{
	class FOpenProjectWindow final : public IBaseUI
	{
	public:
		NODISCARD static FOpenProjectWindow& GetInstance();

	private:
		FORCEINLINE FOpenProjectWindow()
		{
			Setup();
		}

		virtual void Draw() override;

		void Setup();
	};
}
