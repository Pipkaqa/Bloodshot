#pragma once

#include "OpenGL/ImGuiHeader.h"
#include "Platform/Platform.h"
#include "Texture.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace Bloodshot::Launcher
{
	class FWidgetState final
	{
		friend class FGui;

	public:
		NODISCARD FORCEINLINE bool IsHovered()
		{
			return bHovered;
		}

		NODISCARD FORCEINLINE bool IsClicked()
		{
			return bClicked;
		}

	private:
		bool bHovered = false;
		bool bClicked = false;
	};

	struct IWidget abstract
	{
		mutable FWidgetState State;

		ImGuiMouseCursor HoveredCursor = ImGuiMouseCursor_Arrow;
	};

	struct FText final : IWidget
	{
		std::string Value = "";
		ImFont* Font = nullptr;

		ImVec4 Color = {};
		ImVec4 HoveredColor = {};
	};

	struct FButton final : IWidget
	{
		std::string Text = "";
		ImFont* Font = nullptr;

		ImVec4 TextColor = {};
		ImVec4 HoveredTextColor = {};

		ImVec4 Color = {};
		ImVec4 ActiveColor = {};
		ImVec4 HoveredColor = {};

		std::function<void()> OnClickEvent = []() {};
	};

	struct FImage final : IWidget
	{
		FTexture Texture = {};
		ImVec2 Size = {};

		ImVec4 Color = {};
		ImVec4 HoveredColor = {};
	};

	struct FImageButton final : IWidget
	{
		std::string UniqueID = "";

		FTexture Texture = {};
		ImVec2 Size = {};

		ImVec4 ImageColor = {};
		ImVec4 HoveredImageColor = {};

		ImVec4 ButtonColor = {};
		ImVec4 ActiveButtonColor = {};
		ImVec4 HoveredButtonColor = {};

		std::function<void()> OnClickEvent = []() {};
	};

	struct FInputTextBox final : IWidget
	{
		std::string Label = "";
		std::string Hint = "";
		ImFont* Font = nullptr;

		std::shared_ptr<char[]> Buffer = nullptr;
		size_t BufferSize = 0;

		ImVec4 Color = {};
		ImVec4 ActiveColor = {};
		ImVec4 HoveredColor = {};

		ImGuiInputTextFlags Flags = 0;
		ImGuiInputTextCallback Callback = 0;
		void* CallbackData = nullptr;
	};

	struct FLine final
	{
		ImVec4 Color = ImVec4(1.f, 1.f, 1.f, 1.f);
		float Thickness = 1.f;
	};

	struct FTopPanel final
	{
		FButton CloseButton = {};
		FLine CloseButtonIcon[2] = {};

		FButton MaximizeButton = {};
		FLine MaximizeButtonIcon[4] = {};

		FButton MinimizeButton = {};
		FLine MinimizeButtonIcon = {};
	};

	class FWidgetGroup
	{
	public:
		NODISCARD FORCEINLINE size_t GetSize() const
		{
			return Texts.size() + Buttons.size() + Images.size() + ImageButtons.size() + InputTextBoxes.size() + Lines.size();
		}

		NODISCARD FORCEINLINE FText& GetText(const std::string& UniqueID)
		{
			return Texts.at(UniqueID);
		}

		NODISCARD FORCEINLINE const FText& GetText(const std::string& UniqueID) const
		{
			return Texts.at(UniqueID);
		}

		NODISCARD FORCEINLINE FButton& GetButton(const std::string& UniqueID)
		{
			return Buttons.at(UniqueID);
		}

		NODISCARD FORCEINLINE const FButton& GetButton(const std::string& UniqueID) const
		{
			return Buttons.at(UniqueID);
		}

		NODISCARD FORCEINLINE FImage& GetImage(const std::string& UniqueID)
		{
			return Images.at(UniqueID);
		}

		NODISCARD FORCEINLINE const FImage& GetImage(const std::string& UniqueID) const
		{
			return Images.at(UniqueID);
		}

		NODISCARD FORCEINLINE FImageButton& GetImageButton(const std::string& UniqueID)
		{
			return ImageButtons.at(UniqueID);
		}

		NODISCARD FORCEINLINE const FImageButton& GetImageButton(const std::string& UniqueID) const
		{
			return ImageButtons.at(UniqueID);
		}

		NODISCARD FORCEINLINE FInputTextBox& GetInputTextBox(const std::string& UniqueID)
		{
			return InputTextBoxes.at(UniqueID);
		}

		NODISCARD FORCEINLINE const FInputTextBox& GetInputTextBox(const std::string& UniqueID) const
		{
			return InputTextBoxes.at(UniqueID);
		}

		NODISCARD FORCEINLINE FLine& GetLine(const std::string& UniqueID)
		{
			return Lines.at(UniqueID);
		}

		NODISCARD FORCEINLINE const FLine& GetLine(const std::string& UniqueID) const
		{
			return Lines.at(UniqueID);
		}

		FORCEINLINE void SetText(const std::string& UniqueID, FText&& Text)
		{
			Texts.insert_or_assign(UniqueID, std::move(Text));
		}

		FORCEINLINE void SetButton(const std::string& UniqueID, FButton&& Button)
		{
			Buttons.insert_or_assign(UniqueID, std::move(Button));
		}

		FORCEINLINE void SetImage(const std::string& UniqueID, FImage&& Image)
		{
			Images.insert_or_assign(UniqueID, std::move(Image));
		}

		FORCEINLINE void SetImageButton(const std::string& UniqueID, FImageButton&& ImageButton)
		{
			ImageButtons.insert_or_assign(UniqueID, std::move(ImageButton));
		}

		FORCEINLINE void SetInputTextBox(const std::string& UniqueID, FInputTextBox&& InputTextBox)
		{
			InputTextBoxes.insert_or_assign(UniqueID, std::move(InputTextBox));
		}

		FORCEINLINE void SetLine(const std::string& UniqueID, FLine&& Line)
		{
			Lines.insert_or_assign(UniqueID, std::move(Line));
		}

		FORCEINLINE void AddText(std::string&& UniqueID, FText&& Text)
		{
			Texts.emplace(std::move(UniqueID), std::move(Text));
		}

		FORCEINLINE void AddButton(std::string&& UniqueID, FButton&& Button)
		{
			Buttons.emplace(std::move(UniqueID), std::move(Button));
		}

		FORCEINLINE void AddImage(std::string&& UniqueID, FImage&& Image)
		{
			Images.emplace(std::move(UniqueID), std::move(Image));
		}

		FORCEINLINE void AddImageButton(std::string&& UniqueID, FImageButton&& ImageButton)
		{
			ImageButtons.emplace(std::move(UniqueID), std::move(ImageButton));
		}

		FORCEINLINE void AddInputTextBox(std::string&& UniqueID, FInputTextBox&& InputTextBox)
		{
			InputTextBoxes.emplace(std::move(UniqueID), std::move(InputTextBox));
		}

		FORCEINLINE void AddLine(std::string&& UniqueID, FLine&& Line)
		{
			Lines.emplace(std::move(UniqueID), std::move(Line));
		}

	private:
		std::unordered_map<std::string, FText> Texts;
		std::unordered_map<std::string, FButton> Buttons;
		std::unordered_map<std::string, FImage> Images;
		std::unordered_map<std::string, FImageButton> ImageButtons;
		std::unordered_map<std::string, FInputTextBox> InputTextBoxes;
		std::unordered_map<std::string, FLine> Lines;
	};

	class FPage final : public FWidgetGroup
	{
	public:
		ImVec2 TopOffset = ImVec2(24, 36);
		ImVec2 BottomOffset = ImVec2(12, 12);

		NODISCARD FORCEINLINE FWidgetGroup& GetWidgetGroup(const std::string& UniqueID)
		{
			return WidgetGroups.at(UniqueID);
		}

		NODISCARD FORCEINLINE const FWidgetGroup& GetWidgetGroup(const std::string& UniqueID) const
		{
			return WidgetGroups.at(UniqueID);
		}

		FORCEINLINE void SetWidgetGroup(const std::string& UniqueID, FWidgetGroup&& Group)
		{
			WidgetGroups.insert_or_assign(UniqueID, std::move(Group));
		}

		FORCEINLINE void AddWidgetGroup(std::string&& UniqueID, FWidgetGroup&& Group)
		{
			WidgetGroups.emplace(std::move(UniqueID), std::move(Group));
		}

	private:
		std::unordered_map<std::string, FWidgetGroup> WidgetGroups;
	};

	class FWindow
	{
	public:
		ImVec2 TopPanelOffset = ImVec2(12, 12);

		NODISCARD FORCEINLINE FTopPanel& GetTopPanel()
		{
			return TopPanel;
		}

		NODISCARD FORCEINLINE const FTopPanel& GetTopPanel() const
		{
			return TopPanel;
		}

		NODISCARD FORCEINLINE std::unordered_map<std::string, FPage>& GetPages()
		{
			return Pages;
		}

		NODISCARD FORCEINLINE const std::unordered_map<std::string, FPage>& GetPages() const
		{
			return Pages;
		}

		NODISCARD FORCEINLINE FPage& GetPage(const std::string& UniqueID)
		{
			return Pages.at(UniqueID);
		}

		NODISCARD FORCEINLINE const FPage& GetPage(const std::string& UniqueID) const
		{
			return Pages.at(UniqueID);
		}

		FORCEINLINE void SetTopPanel(FTopPanel&& TopPanel)
		{
			this->TopPanel = std::move(TopPanel);
		}

		FORCEINLINE void SetPage(const std::string& UniqueID, FPage&& Page)
		{
			Pages.insert_or_assign(UniqueID, std::move(Page));
		}

	private:
		FTopPanel TopPanel;
		std::unordered_map<std::string, FPage> Pages;
	};
}
