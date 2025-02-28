#pragma once

namespace Bloodshot::Launcher
{
	class IBaseUI
	{
		friend class FLauncher;

	public:
		IBaseUI() {}
		IBaseUI(const IBaseUI&) = delete;
		IBaseUI(IBaseUI&&) = delete;
		IBaseUI& operator=(const IBaseUI&) = delete;
		IBaseUI& operator=(IBaseUI&&) = delete;
		virtual ~IBaseUI() {}

	protected:
		virtual void Draw() = 0;
	};
}
