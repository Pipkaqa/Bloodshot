#pragma once

#include "Rendering/Window.h"

struct GLFWwindow;

namespace Bloodshot
{
	class OpenGLWindow final : public Window
	{
		OWNED_BY_CORE;
		RENDERING_PART;

	public:
		NODISCARD int GetWidth() const noexcept override;
		NODISCARD int GetHeight() const noexcept override;
		NODISCARD int GetFPS() const noexcept override;

		NODISCARD bool ShouldClose() const noexcept override;

		void SwapBuffers() const override;
		void PollEvents() const override;
		void Close() noexcept override;

	private:
		GLFWwindow* m_Window = nullptr;

		void Init() override;
		void Dispose() override;
	};
}
