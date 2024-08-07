#pragma once

#include "Window.h"

struct GLFWwindow;

namespace Bloodshot
{
	class FOpenGLWindow final : public IWindow
	{
	public:
		FOpenGLWindow(const char* WindowName = "Bloodshot Game Engine", const glm::uvec2& WindowSize = {640U, 480U}, const bool bVSyncEnabled = false);

		GLFWwindow* Window = nullptr;

		virtual void Init() override;
		virtual void Dispose() override;

		NODISCARD virtual bool ShouldClose() const override;

		virtual void SetVSync(const bool bEnabled) override;

		virtual void SwapBuffers() override;
		virtual void PollEvents() override;
		virtual void Close() override;
	};
}
