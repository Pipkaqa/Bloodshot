#pragma once

#include "Rendering/Window.h"

struct GLFWwindow;

namespace Bloodshot
{
	class OpenGLWindow final : public Window
	{
		RENDERER_MODULE;

		GLFWwindow* m_Window = nullptr;

		NODISCARD uint32_t GetWidth() const noexcept override;
		NODISCARD uint32_t GetHeight() const noexcept override;

		void SetVSync(bool vsync) override;

		NODISCARD bool ShouldClose() const noexcept override;

		void Init() override;
		void Dispose() override;

		void SwapBuffers() const override;
		void PollEvents() const override;
		void Close() noexcept override;
	};
}
