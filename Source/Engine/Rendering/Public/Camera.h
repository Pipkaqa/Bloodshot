#pragma once

#include "MathLibrary.h"
#include "Platform/Platform.h"
#include "Window.h"

#include <cstdint>

namespace Bloodshot
{
	enum class ECameraProjectionMode : uint8_t
	{
		Orthographic = 0,
		Perspective
	};

	struct FCameraSettings final
	{
		ECameraProjectionMode ProjectionMode = ECameraProjectionMode::Perspective;

		float Fov = 45.f;
		float AspectRatio = (float)IWindow::GetWidth() / (float)IWindow::GetHeight();
		float Near = 0.001f;
		float Far = 100.f;
		float Left = 0.f;
		float Right = (float)IWindow::GetWidth();
		float Bottom = 0.f;
		float Top = (float)IWindow::GetHeight();
	};

	class ICamera abstract
	{
	public:
		ICamera(const FCameraSettings& Settings = {});
		virtual ~ICamera() {}

		NODISCARD FORCEINLINE const glm::mat4& GetProjectionMatrix()
		{
			if (bDirty)
			{
				RecalculateProjectionMatrix();
			}

			return ProjectionMatrix;
		}

		NODISCARD FORCEINLINE ECameraProjectionMode GetProjectionMode() const noexcept
		{
			return Settings.ProjectionMode;
		}

		NODISCARD FORCEINLINE float GetFov() const noexcept
		{
			return Settings.Fov;
		}

		NODISCARD FORCEINLINE float GetAspectRatio() const noexcept
		{
			return Settings.AspectRatio;
		}

		NODISCARD FORCEINLINE float GetNearPlain() const noexcept
		{
			return Settings.Near;
		}

		NODISCARD FORCEINLINE float GetFarPlain() const noexcept
		{
			return Settings.Far;
		}

		NODISCARD FORCEINLINE float GetLeft() const noexcept
		{
			return Settings.Left;
		}

		NODISCARD FORCEINLINE float GetRight() const noexcept
		{
			return Settings.Right;
		}

		NODISCARD FORCEINLINE float GetBottom() const noexcept
		{
			return Settings.Bottom;
		}

		NODISCARD FORCEINLINE float GetTop() const noexcept
		{
			return Settings.Top;
		}

		void SetProjectionMode(const ECameraProjectionMode NewValue) noexcept
		{
			ECameraProjectionMode& ProjectionMode = Settings.ProjectionMode;

			if (ProjectionMode != NewValue)
			{
				ProjectionMode = NewValue;
				bDirty = true;
			}
		}

		void SetFov(const float NewValue) noexcept
		{
			float& Fov = Settings.Fov;

			if (Fov != NewValue)
			{
				Fov = NewValue;
				bDirty = true;
			}
		}

		void SetAspectRatio(const float NewValue) noexcept
		{
			float& AspectRatio = Settings.AspectRatio;

			if (AspectRatio != NewValue)
			{
				AspectRatio = NewValue;
				bDirty = true;
			}
		}

		void SetNearPlain(const float NewValue) noexcept
		{
			float& Near = Settings.Near;

			if (Near != NewValue)
			{
				Near = NewValue;
				bDirty = true;
			}
		}

		void SetFarPlain(const float NewValue) noexcept
		{
			float& Far = Settings.Far;

			if (Far != NewValue)
			{
				Far = NewValue;
				bDirty = true;
			}
		}

		void SetLeft(const float NewValue) noexcept
		{
			float& Left = Settings.Left;

			if (Left != NewValue)
			{
				Left = NewValue;
				bDirty = true;
			}
		}

		void SetRight(const float NewValue) noexcept
		{
			float& Right = Settings.Right;

			if (Right != NewValue)
			{
				Right = NewValue;
				bDirty = true;
			}
		}

		void SetBottom(const float NewValue) noexcept
		{
			float& Bottom = Settings.Bottom;

			if (Bottom != NewValue)
			{
				Bottom = NewValue;
				bDirty = true;
			}
		}

		void SetTop(const float NewValue) noexcept
		{
			float& Top = Settings.Top;

			if (Top != NewValue)
			{
				Top = NewValue;
				bDirty = true;
			}
		}

	protected:
		FCameraSettings Settings;
		glm::mat4 ProjectionMatrix;
		bool bDirty = false;

		FORCEINLINE void RecalculateProjectionMatrix()
		{
			ProjectionMatrix = Settings.ProjectionMode == ECameraProjectionMode::Perspective
				? glm::perspective(Settings.Fov, Settings.AspectRatio, Settings.Near, Settings.Far)
				: glm::ortho(Settings.Left, Settings.Right, Settings.Bottom, Settings.Top);
		}
	};
}
