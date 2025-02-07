#pragma once

#include "Constants.h"
#include "OpenGL/ImGuiHeader.h"
#include "Widgets.h"

#include <list>
#include <vector>

namespace Bloodshot::Launcher
{
	struct FTexture;

	struct FDrawnWidgetRecord final
	{
		const IWidget* Widget = nullptr;
		ImGuiID UniqueID = 0;
		std::string Type = {};
		std::string Info = {};
		ImVec2 Position = {};
		bool bHovered = false;
		bool bClicked = false;
	};

	class FGui final
	{
	public:
		FGui();
		~FGui();

		NODISCARD FORCEINLINE static const bool IsRunning()
		{
			return !glfwWindowShouldClose(Instance->Window);
		}

		FORCEINLINE static void Shutdown()
		{
			glfwSetWindowShouldClose(Instance->Window, GLFW_TRUE);
		}

		NODISCARD FORCEINLINE static int GetKey(const int Key)
		{
			return glfwGetKey(Instance->Window, Key);
		}

		NODISCARD FORCEINLINE static const ImVec2& GetWindowPosition()
		{
			return Instance->WindowPosition;
		}

		NODISCARD FORCEINLINE static const ImVec2& GetWindowSize()
		{
			return Instance->WindowSize;
		}

		NODISCARD FORCEINLINE static const ImVec2& GetMousePosition()
		{
			return ImGui::GetIO().MousePos;
		}

		NODISCARD FORCEINLINE static ImVec2 GetRelativeMousePosition()
		{
			return ImGui::GetIO().MousePos - Instance->WindowPosition;
		}

		NODISCARD FORCEINLINE static ImVec2 GetCursorPosition()
		{
			return ImGui::GetCursorPos();
		}

		NODISCARD FORCEINLINE static const ImVec2& GetFramePadding()
		{
			return ImGui::GetStyle().FramePadding;
		}

		NODISCARD FORCEINLINE static FPage& GetPage(const std::string& UniqueID)
		{
			return Instance->Pages.at(UniqueID).Page;
		}

		NODISCARD FORCEINLINE static ImFont* GetFont(const float Size)
		{
			return Instance->Fonts.at(Size);
		}

		NODISCARD FORCEINLINE static FTexture& GetTexture(const std::string& UniqueID)
		{
			return Instance->Textures.at(UniqueID);
		}

		NODISCARD FORCEINLINE static const std::list<FDrawnWidgetRecord>& GetDrawnWidgetRecordStack()
		{
			return Instance->LastDrawnWidgetRecords;
		}

		static void BeginRender();
		static void EndRender();

		static bool BeginWindow(const char* WindowName, ImGuiWindowFlags Flags = 0, const ImVec2& MinSize = ImVec2(-1.f, -1.f), bool* bOpened = nullptr);
		static void EndWindow();

		FORCEINLINE static void EnableDebugMode()
		{
			Instance->bDebugMode = true;
		}

		FORCEINLINE static void DisableDebugMode()
		{
			Instance->bDebugMode = false;
		}

		FORCEINLINE static void AddPage(std::string&& UniqueID, FPage&& Page, std::function<void()>&& DrawFunction)
		{
			Instance->Pages.emplace(std::move(UniqueID), FPageDescription(std::move(Page), std::move(DrawFunction)));
		}

		FORCEINLINE static void LoadFontFromFile(const char* Path, const float Size)
		{
			Instance->Fonts.emplace(Size, ImGui::GetIO().Fonts->AddFontFromFileTTF(Path, Size));
		}

		static void LoadTextureFromFile(std::string&& UniqueID, const char* Path);

		static ImVec2 CalculateSize(const FText& Text);
		static ImVec2 CalculateSize(const FButton& Button);
		static ImVec2 CalculateSize(const FImage& Image);
		static ImVec2 CalculateSize(const FImageButton& ImageButton);
		static ImVec2 CalculateSize(const FInputTextBox& InputTextBox);

		FORCEINLINE static void DrawPage(const std::string& UniqueID)
		{
			Instance->Pages[UniqueID].DrawFunction();
		}

		static void Draw(const FText& Text);
		static void Draw(const FButton& Button, const ImVec2& Size = ImVec2(0.f, 0.f));
		static void Draw(const FImage& Image);
		static void Draw(const FImageButton& ImageButton);
		static void Draw(const FInputTextBox& InputTextBox);
		static void Draw(const FLine& Line, const ImVec2& StartPosition, const ImVec2& EndPosition);
		static void Draw(const FLine& Line, const ImVec2& StartPosition, const float EndPositionX, const float EndPositionY);
		static void Draw(const FLine& Line, const float StartPositionX, const float StartPositionY, const ImVec2& EndPosition);
		static void Draw(const FLine& Line, const float StartPositionX, const float StartPositionY,
			const float EndPositionX, const float EndPositionY);

		static const FWidgetState& UpdateState(const uint8_t LastDrawnWidgetsCount = 1);

		FORCEINLINE static void SetCursorPosition(const ImVec2& Position)
		{
			ImGui::SetCursorPos(Position);
		}

		FORCEINLINE static void SetCursorPosition(const float PositionX, const float PositionY)
		{
			ImGui::SetCursorPos(ImVec2(PositionX, PositionY));
		}

		FORCEINLINE static void SetCursorPositionX(const float Position)
		{
			ImGui::SetCursorPosX(Position);
		}

		FORCEINLINE static void SetCursorPositionY(const float Position)
		{
			ImGui::SetCursorPosY(Position);
		}

		FORCEINLINE static void MoveCursor(const ImVec2& Offset)
		{
			ImGui::SetCursorPos(ImGui::GetCursorPos() + Offset);
		}

		FORCEINLINE static void MoveCursor(const float PositionX, const float PositionY)
		{
			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(PositionX, PositionY));
		}

		FORCEINLINE static void MoveCursorX(const float Position)
		{
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + Position);
		}

		FORCEINLINE static void MoveCursorY(const float Position)
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + Position);
		}

		FORCEINLINE static void SetFramePaddingMultiplier(const uint8_t Multiplier)
		{
			ImGui::GetStyle().FramePadding = Instance->DefaultFramePadding * Multiplier;
		}

		FORCEINLINE static void KeepLine(float OffsetX = 0.f, float Spacing = 0.f)
		{
			ImGui::SameLine(OffsetX, Spacing);
		}

	private:

		// BSTODO: Rewrite

		struct FPageDescription final
		{
			FPage Page;
			std::function<void()> DrawFunction;
		};

		static inline FGui* Instance = nullptr;

		GLFWwindow* Window = nullptr;

		ImVec2 WindowPosition;
		ImVec2 WindowSize = IConstants::MinWindowSize;

		std::unordered_map<std::string, FPageDescription> Pages;
		std::unordered_map<std::string, FTexture> Textures;
		std::unordered_map<float, ImFont*> Fonts;

		std::list<FDrawnWidgetRecord> LastDrawnWidgetRecords;

		ImVec2 DefaultFramePadding;

		bool bDebugMode = false;

		static void WriteDrawnWidgetRecord(const FText& Text);
		static void WriteDrawnWidgetRecord(const FButton& Button);
		static void WriteDrawnWidgetRecord(const FImage& Image);
		static void WriteDrawnWidgetRecord(const FImageButton& ImageButton);
		static void WriteDrawnWidgetRecord(const FInputTextBox& InputTextBox);
		static void WriteRecord(FDrawnWidgetRecord&& Record);

		FORCEINLINE static void PushStyleColor(ImGuiCol Index, const ImVec4& Color)
		{
			ImGui::PushStyleColor(Index, Color);
		}

		FORCEINLINE static void PushStyleVariable(ImGuiStyleVar Index, const ImVec2& Value)
		{
			ImGui::PushStyleVar(Index, Value);
		}

		FORCEINLINE static void PushFont(ImFont* Font)
		{
			ImGui::PushFont(Font);
		}

		FORCEINLINE static void PopStyleColor(const uint8_t Count = 1)
		{
			ImGui::PopStyleColor(Count);
		}

		FORCEINLINE static void PopStyleVariable(const uint8_t Count = 1)
		{
			ImGui::PopStyleVar(Count);
		}

		FORCEINLINE static void PopFont()
		{
			ImGui::PopFont();
		}
	};
}
