#include "GUI.h"
#include "OpenGL/OpenGLHeader.h"
#include "StbImage.h"
#include "Texture.h"

#include <ranges>

namespace Bloodshot::Launcher
{
	FGui::FGui()
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

		Window = glfwCreateWindow(4, 4, "W", nullptr, nullptr);

		if (!Window)
		{
			glfwTerminate();
			return;
		}

		glfwMakeContextCurrent(Window);
		glfwSetWindowSizeCallback(Window,
			[](GLFWwindow* const Window, const int Width, const int Height) { glViewport(0, 0, Width, Height); });

		if (!gladLoadGL())
		{
			glfwTerminate();
			return;
		}

		ImGui::CreateContext();

		ImGui_ImplOpenGL3_Init("#version 460");
		ImGui_ImplGlfw_InitForOpenGL(Window, true);

		ImGui::StyleColorsDark();

		DefaultFramePadding = ImGui::GetStyle().FramePadding;

		ImGuiIO& GuiIO = ImGui::GetIO();
		GuiIO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		GuiIO.Fonts->AddFontDefault();
	}

	FGui::~FGui()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwTerminate();
	}

	NODISCARD FGui& FGui::GetInstance()
	{
		static FGui Instance;
		return Instance;
	}

	void FGui::BeginRender()
	{
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// BSTODO: Enable DebugMode ?
	}

	void FGui::EndRender()
	{
		// BSTODO: Disable DebugMode ?

		GetInstance().LastDrawnWidgetRecords.clear();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		GLFWwindow* CurrentContext = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(CurrentContext);

		glfwSwapBuffers(GetInstance().Window);
	}

	bool FGui::BeginWindow(const char* WindowName, ImGuiWindowFlags Flags, const ImVec2& MinSize, bool* bOpened)
	{
		if (MinSize.x >= 0.f && MinSize.y >= 0.f)
		{
			FGui::PushStyleVariable(ImGuiStyleVar_WindowMinSize, MinSize);
		}

		const bool Result = ImGui::Begin(WindowName, bOpened,
			Flags | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);

		GetInstance().WindowPosition = ImGui::GetWindowPos();
		GetInstance().WindowSize = ImGui::GetWindowSize();

		return Result;
	}

	void FGui::EndWindow()
	{
		if (GetInstance().bDebugMode)
		{
			FLine DebugLines[2];

			DebugLines[0].Color = DebugLines[1].Color = ImVec4(0.f, 1.f, 0.f, 0.5f);

			const ImVec2& WindowPosition = GetInstance().WindowPosition;
			const ImVec2& WindowSize = GetInstance().WindowSize;
			const ImVec2& MousePosition = GetMousePosition();

			Draw(DebugLines[0], ImVec2(WindowPosition.x, MousePosition.y), ImVec2(WindowPosition.x + WindowSize.x, MousePosition.y));
			Draw(DebugLines[1], ImVec2(MousePosition.x, WindowPosition.y), ImVec2(MousePosition.x, WindowPosition.y + WindowSize.y));
		}

		ImGui::PopStyleVar(ImGui::GetCurrentContext()->StyleVarStack.Size);
		ImGui::End();
	}

	void FGui::LoadTextureFromFile(std::string&& UniqueID, const char* Path)
	{
		FTexture Texture;
		GLuint& TextureUniqueID = Texture.UniqueID;
		Texture.Name = (const std::string&)UniqueID;

		int& Width = Texture.Width;
		int& Height = Texture.Height;
		int& Channels = Texture.Channels;

		uint8_t* const Data = stbi_load(Path, &Width, &Height, &Channels, STBI_rgb_alpha);

		glCreateTextures(GL_TEXTURE_2D, 1, &TextureUniqueID);
		glTextureStorage2D(TextureUniqueID, 1, GL_RGBA8, Width, Height);
		glTextureSubImage2D(TextureUniqueID, 0, 0, 0, Width, Height, GL_RGBA, GL_UNSIGNED_BYTE, Data);
		glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateTextureMipmap(TextureUniqueID);

		stbi_image_free(Data);

		GetInstance().Textures.emplace(std::move(UniqueID), std::move(Texture));
	}

	ImVec2 FGui::CalculateSize(const FText& Text)
	{
		ImGui::PushFont(Text.Font);
		const ImVec2& TextSize = ImGui::CalcTextSize(Text.Value.c_str());
		ImGui::PopFont();

		return ImGui::CalcItemSize(ImVec2(), TextSize.x, TextSize.y);
	}

	ImVec2 FGui::CalculateSize(const FButton& Button)
	{
		ImGui::PushFont(Button.Font);
		const ImVec2& RawSize = ImGui::CalcTextSize(Button.Text.c_str()) + ImGui::GetStyle().FramePadding * 2.f;
		ImGui::PopFont();

		return ImGui::CalcItemSize(ImVec2(), RawSize.x, RawSize.y);
	}

	ImVec2 FGui::CalculateSize(const FImage& Image)
	{
		// BSTODO: Border color and paddings for Image
		const ImVec4& BorderColor = ImVec4();
		const ImVec2& Padding = BorderColor.w > 0.f ? ImVec2(1.f, 1.f) : ImVec2(0.f, 0.f);
		const ImVec2& RawSize = Image.Size + Padding * 2.f;

		return ImGui::CalcItemSize(ImVec2(), RawSize.x, RawSize.y);
	}

	ImVec2 FGui::CalculateSize(const FImageButton& ImageButton)
	{
		const ImVec2& RawSize = ImageButton.Size + ImGui::GetStyle().FramePadding * 2.f;
		return ImGui::CalcItemSize(ImVec2(), RawSize.x, RawSize.y);
	}

	ImVec2 FGui::CalculateSize(const FInputTextBox& InputTextBox)
	{
		ImGui::PushFont(InputTextBox.Font);
		const ImVec2& TextSize = ImGui::CalcTextSize(InputTextBox.Label.c_str());
		ImGui::PopFont();

		return ImGui::CalcItemSize(ImVec2(), TextSize.x + TextSize.x > 0.f ? ImGui::GetStyle().ItemInnerSpacing.x : 0.f, TextSize.y)
			+ ImGui::GetStyle().FramePadding * 2.f;
	}

	// BSTODO: CursorType is overriding by last Draw() call, fix this

	void FGui::Draw(const FText& Text)
	{
		const bool bHovered = Text.State.bHovered;

		bHovered ? ImGui::SetMouseCursor(Text.HoveredCursor) : void();
		ImGui::PushFont(Text.Font);
		ImGui::PushStyleColor(ImGuiCol_Text, bHovered ? Text.HoveredColor : Text.Color);
		if (GetInstance().bDebugMode)
		{
			const ImVec2& CursorGlobalPosition = GetWindowPosition() + GetCursorPosition();

			ImGui::RenderFrame(CursorGlobalPosition,
				CursorGlobalPosition + CalculateSize(Text),
				ImGui::ColorConvertFloat4ToU32(ImVec4(0.f, 1.f, 0.f, 0.5f)),
				true,
				ImGui::GetStyle().FrameRounding);
		}
		ImGui::Text(Text.Value.c_str());
		ImGui::PopStyleColor(ImGui::GetCurrentContext()->ColorStack.Size);
		ImGui::PopFont();

		WriteDrawnWidgetRecord(Text);
	}

	void FGui::Draw(const FButton& Button, const ImVec2& Size)
	{
		const bool bHovered = Button.State.bHovered;
		const ImVec4& Color = Button.Color;

		bHovered ? ImGui::SetMouseCursor(Button.HoveredCursor) : void();
		ImGui::PushFont(Button.Font);
		ImGui::PushStyleColor(ImGuiCol_Text, bHovered ? Button.HoveredTextColor : Button.TextColor);
		ImGui::PushStyleColor(ImGuiCol_Button,
			GetInstance().bDebugMode ? Color.w > 0.f ? Color : ImVec4(0.f, 1.f, 0.f, 0.5f) : Color);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, Button.ActiveColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Button.HoveredColor);
		ImGui::Button(Button.Text.c_str(), Size) ? Button.OnClickEvent() : void();
		ImGui::PopStyleColor(ImGui::GetCurrentContext()->ColorStack.Size);
		ImGui::PopFont();

		WriteDrawnWidgetRecord(Button);
	}

	void FGui::Draw(const FImage& Image)
	{
		const bool bHovered = Image.State.bHovered;

		bHovered ? ImGui::SetMouseCursor(Image.HoveredCursor) : void();
		ImGui::Image(Image.Texture.UniqueID,
			Image.Size,
			ImVec2(),
			ImVec2(1.f, 1.f),
			bHovered ? Image.HoveredColor : Image.Color,
			GetInstance().bDebugMode ? ImVec4(0.f, 1.f, 0.f, 0.5f) : ImVec4(0.f, 0.f, 0.f, 0.f));

		WriteDrawnWidgetRecord(Image);
	}

	void FGui::Draw(const FImageButton& ImageButton)
	{
		const bool bHovered = ImageButton.State.bHovered;

		bHovered ? ImGui::SetMouseCursor(ImageButton.HoveredCursor) : void();
		ImGui::PushStyleColor(ImGuiCol_Button, ImageButton.ButtonColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImageButton.ActiveButtonColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImageButton.HoveredButtonColor);
		ImGui::ImageButton(ImageButton.UniqueID.c_str(),
			ImageButton.Texture.UniqueID,
			ImageButton.Size,
			ImVec2(),
			ImVec2(1.f, 1.f),
			GetInstance().bDebugMode ? ImVec4(0.f, 1.f, 0.f, 0.5f) : ImVec4(0.f, 0.f, 0.f, 0.f),
			bHovered ? ImageButton.HoveredImageColor : ImageButton.ImageColor) ? ImageButton.OnClickEvent() : void();
		ImGui::PopStyleColor(ImGui::GetCurrentContext()->ColorStack.Size);

		WriteDrawnWidgetRecord(ImageButton);
	}

	void FGui::Draw(const FInputTextBox& InputTextBox)
	{
		const bool bHovered = InputTextBox.State.bHovered;
		const ImVec4& Color = InputTextBox.Color;

		bHovered ? ImGui::SetMouseCursor(InputTextBox.HoveredCursor) : void();
		ImGui::PushFont(InputTextBox.Font);
		ImGui::PushStyleColor(ImGuiCol_FrameBg,
			GetInstance().bDebugMode ? Color.w > 0.f ? Color : ImVec4(0.f, 1.f, 0.f, 0.5f) : Color);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, InputTextBox.ActiveColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, InputTextBox.HoveredColor);
		ImGui::InputTextEx(InputTextBox.Label.c_str(),
			InputTextBox.Hint.c_str(),
			InputTextBox.Buffer.get(),
			(int)InputTextBox.BufferSize,
			ImVec2(),
			0);
		ImGui::PopStyleColor(ImGui::GetCurrentContext()->ColorStack.Size);
		ImGui::PopFont();

		WriteDrawnWidgetRecord(InputTextBox);
	}

	void FGui::Draw(const FLine& Line, const ImVec2& StartPosition, const ImVec2& EndPosition)
	{
		const ImVec2& WindowPosition = GetInstance().WindowPosition;
		ImGui::GetWindowDrawList()->AddLine(StartPosition,
			EndPosition,
			(ImColor)Line.Color,
			Line.Thickness);
	}

	void FGui::Draw(const FLine& Line, const ImVec2& StartPosition, const float EndPositionX, const float EndPositionY)
	{
		Draw(Line, StartPosition, ImVec2(EndPositionX, EndPositionY));
	}

	void FGui::Draw(const FLine& Line, const float StartPositionX, const float StartPositionY, const ImVec2& EndPosition)
	{
		Draw(Line, ImVec2(StartPositionX, StartPositionY), EndPosition);
	}

	void FGui::Draw(const FLine& Line, const float StartPositionX, const float StartPositionY,
		const float EndPositionX, const float EndPositionY)
	{
		Draw(Line, ImVec2(StartPositionX, StartPositionY), ImVec2(EndPositionX, EndPositionY));
	}

	void FGui::DrawTopPanel(const FWindow& Window)
	{
		const FTopPanel& TopPanel = Window.GetTopPanel();
		const ImVec2& TopPanelOffset = Window.TopPanelOffset;
		const FButton& CloseButton = TopPanel.CloseButton;
		const float CloseButtonFontSize = CloseButton.Font->FontSize;
		const ImVec2& CloseButtonSize = ImVec2(CloseButtonFontSize, CloseButtonFontSize);
		const ImVec2& CloseButtonLocalPosition =
			ImVec2(FGui::GetWindowSize().x - CloseButtonSize.x - TopPanelOffset.x, TopPanelOffset.y);

		FGui::SetCursorPosition(CloseButtonLocalPosition);

		const ImVec2& WindowPosition = FGui::GetWindowPosition();
		const ImVec2& CrossCenterLocalPosition = CloseButtonLocalPosition + CloseButtonSize * 0.5f - ImVec2(0.5f, 0.5f);
		const ImVec2& CrossCenterGlobalPosition = WindowPosition + CrossCenterLocalPosition;

		const FLine* const CloseButtonIcon = TopPanel.CloseButtonIcon;
		const float CrossExtent = CloseButtonSize.x * 0.5f * 0.7071f - 1.0f;

		FGui::Draw(CloseButton, CloseButtonSize);
		FGui::Draw(CloseButtonIcon[0],
			CrossCenterGlobalPosition + ImVec2(+CrossExtent, +CrossExtent),
			CrossCenterGlobalPosition + ImVec2(-CrossExtent, -CrossExtent));
		FGui::Draw(CloseButtonIcon[1],
			CrossCenterGlobalPosition + ImVec2(+CrossExtent, -CrossExtent),
			CrossCenterGlobalPosition + ImVec2(-CrossExtent, +CrossExtent));

		const ImVec2& FramePadding = FGui::GetFramePadding();
		const FButton& MaximizeButton = TopPanel.MaximizeButton;
		const float MaximizeButtonFontSize = MaximizeButton.Font->FontSize;
		const ImVec2& MaximizeButtonSize = ImVec2(MaximizeButtonFontSize, MaximizeButtonFontSize);
		const ImVec2& MaximizeButtonLocalPosition =
			ImVec2(CloseButtonLocalPosition.x - MaximizeButtonSize.x - FramePadding.x, TopPanelOffset.y);

		FGui::SetCursorPosition(MaximizeButtonLocalPosition);

		const ImVec2& SquareCenterLocalPosition =
			MaximizeButtonLocalPosition + MaximizeButtonSize * 0.5f - ImVec2(0.5f, 0.5f);
		const ImVec2& SquareCenterGlobalPosition = WindowPosition + SquareCenterLocalPosition;
		const float SquareExtent = MaximizeButtonSize.x * 0.5f * 0.7071f - 1.0f;

		const FLine* const MaximizeButtonIcon = TopPanel.MaximizeButtonIcon;

		FGui::Draw(MaximizeButton, MaximizeButtonSize);
		FGui::Draw(MaximizeButtonIcon[0],
			SquareCenterGlobalPosition + ImVec2(+SquareExtent, -SquareExtent),
			SquareCenterGlobalPosition + ImVec2(-SquareExtent, -SquareExtent));
		FGui::Draw(MaximizeButtonIcon[1],
			SquareCenterGlobalPosition + ImVec2(-SquareExtent, -SquareExtent),
			SquareCenterGlobalPosition + ImVec2(-SquareExtent, +SquareExtent));
		FGui::Draw(MaximizeButtonIcon[2],
			SquareCenterGlobalPosition + ImVec2(-SquareExtent, +SquareExtent),
			SquareCenterGlobalPosition + ImVec2(+SquareExtent, +SquareExtent));
		FGui::Draw(MaximizeButtonIcon[3],
			SquareCenterGlobalPosition + ImVec2(+CrossExtent, +CrossExtent),
			SquareCenterGlobalPosition + ImVec2(+CrossExtent, -CrossExtent));

		const FButton& MinimizeButton = TopPanel.MinimizeButton;
		const float MinimizeButtonFontSize = MinimizeButton.Font->FontSize;
		const ImVec2& MinimizeButtonSize = ImVec2(MinimizeButtonFontSize, MinimizeButtonFontSize);
		const ImVec2& MinimizeButtonLocalPosition = ImVec2(CloseButtonLocalPosition.x
			- MinimizeButtonSize.x
			- MaximizeButtonSize.x
			- FramePadding.x * 2.f,
			TopPanelOffset.y);

		FGui::SetCursorPosition(MinimizeButtonLocalPosition);

		const ImVec2& LineCenterLocalPosition = MinimizeButtonLocalPosition + MinimizeButtonSize * 0.5f - ImVec2(0.5f, 0.5f);
		const ImVec2& LineCenterGlobalPosition = WindowPosition + LineCenterLocalPosition;
		const float LineExtent = MinimizeButtonSize.x * 0.5f * 0.7071f - 1.0f;

		const FLine MinimizeButtonIcon = TopPanel.MinimizeButtonIcon;

		FGui::Draw(MinimizeButton, MinimizeButtonSize);
		FGui::UpdateState();
		FGui::Draw(MinimizeButtonIcon,
			ImVec2(LineCenterGlobalPosition.x + LineExtent, LineCenterGlobalPosition.y),
			ImVec2(LineCenterGlobalPosition.x - LineExtent, LineCenterGlobalPosition.y));
	}

	FTopPanel FGui::CreateTopPanel(std::function<void()>&& CloseButtonFunc,
		std::function<void()>&& MaximizeButtonFunc,
		std::function<void()>&& MinimizeButtonFunc)
	{
		FTopPanel TopPanel;

		FButton& CloseButton = TopPanel.CloseButton;
		CloseButton.Text = "##Close";
		CloseButton.Font = FGui::GetFont(16);
		CloseButton.TextColor = ImVec4(1.f, 1.f, 1.f, 1.f);
		CloseButton.HoveredTextColor = CloseButton.TextColor;
		CloseButton.Color = ImVec4();
		CloseButton.ActiveColor = ImVec4(0.8f, 0.f, 0.2f, 1.f);
		CloseButton.HoveredColor = ImVec4(1.f, 0.f, 0.f, 1.f);
		CloseButton.OnClickEvent = std::move(CloseButtonFunc);

		FButton& MaximizeButton = TopPanel.MaximizeButton;
		MaximizeButton.Text = "##Fullscreen";
		MaximizeButton.Font = CloseButton.Font;
		MaximizeButton.TextColor = ImVec4(1.f, 1.f, 1.f, 1.f);
		MaximizeButton.HoveredTextColor = MaximizeButton.TextColor;
		MaximizeButton.Color = ImVec4();
		MaximizeButton.ActiveColor = ImVec4(0.1f, 0.f, 0.7f, 1.f);
		MaximizeButton.HoveredColor = ImVec4(0.1f, 0.f, 0.9f, 1.f);
		MaximizeButton.OnClickEvent = std::move(MaximizeButtonFunc);

		FButton& MinimizeButton = TopPanel.MinimizeButton;
		MinimizeButton.Text = "##Minimize";
		MinimizeButton.Font = CloseButton.Font;
		MinimizeButton.TextColor = ImVec4(1.f, 1.f, 1.f, 1.f);
		MinimizeButton.HoveredTextColor = MinimizeButton.TextColor;
		MinimizeButton.Color = ImVec4();
		MinimizeButton.ActiveColor = ImVec4(0.1f, 0.f, 0.7f, 1.f);
		MinimizeButton.HoveredColor = ImVec4(0.1f, 0.f, 0.9f, 1.f);
		MinimizeButton.OnClickEvent = std::move(MinimizeButtonFunc);

		return TopPanel;
	}

	void FGui::WriteDrawnWidgetRecord(const FText& Text)
	{
		FDrawnWidgetRecord Record;
		Record.Widget = &Text;
		Record.Type = "Text";
		Record.Info = Text.Value;

		WriteRecord(std::move(Record));
	}

	void FGui::WriteDrawnWidgetRecord(const FButton& Button)
	{
		FDrawnWidgetRecord Record;
		Record.Widget = &Button;
		Record.Type = "Button";
		Record.Info = Button.Text;

		WriteRecord(std::move(Record));
	}

	void FGui::WriteDrawnWidgetRecord(const FImage& Image)
	{
		FDrawnWidgetRecord Record;
		Record.Widget = &Image;
		Record.Type = "Image";
		Record.Info = Image.Texture.Name;

		WriteRecord(std::move(Record));
	}

	void FGui::WriteDrawnWidgetRecord(const FImageButton& ImageButton)
	{
		FDrawnWidgetRecord Record;
		Record.Widget = &ImageButton;
		Record.Type = "ImageButton";
		Record.Info = ImageButton.UniqueID;

		WriteRecord(std::move(Record));
	}

	void FGui::WriteDrawnWidgetRecord(const FInputTextBox& InputTextBox)
	{
		FDrawnWidgetRecord Record;
		Record.Widget = &InputTextBox;
		Record.Type = "InputTextBox";
		Record.Info = InputTextBox.Label;

		WriteRecord(std::move(Record));
	}

	void FGui::WriteRecord(FDrawnWidgetRecord&& Record)
	{
		Record.UniqueID = ImGui::GetItemID();
		Record.Position = ImGui::GetCurrentWindow()->DC.CursorPosPrevLine;
		Record.bHovered = ImGui::IsItemHovered();
		Record.bClicked = ImGui::IsItemClicked();

		GetInstance().LastDrawnWidgetRecords.emplace_back(std::move(Record));
	}

	const FWidgetState& FGui::UpdateState(const uint8_t LastDrawnWidgetsCount)
	{
		std::list<FDrawnWidgetRecord>& LastDrawnWidgetRecords = GetInstance().LastDrawnWidgetRecords;

		assert(LastDrawnWidgetsCount <= LastDrawnWidgetRecords.size() && "FGui::UpdateState: Drawn widget stack size exceeded in UpdateHovering()");

		namespace ranges = std::ranges;

		auto RecordsView = ranges::views::take(ranges::views::reverse(LastDrawnWidgetRecords), LastDrawnWidgetsCount);

		auto HoveredResultIt = ranges::find_if(RecordsView, [](const FDrawnWidgetRecord& Record)
		{
			const bool bWasHovered = Record.bHovered;
			Record.Widget->State.bHovered = bWasHovered;
			return bWasHovered;
		});

		if (HoveredResultIt != RecordsView.end())
		{
			for (FDrawnWidgetRecord& Record : RecordsView)
			{
				Record.Widget->State.bHovered = true;
				Record.bHovered = true;
			}
		}

		auto ClickedResultIt = ranges::find_if(RecordsView, [](const FDrawnWidgetRecord& Record)
		{
			const bool bWasClicked = Record.bClicked;
			Record.Widget->State.bClicked = bWasClicked;
			return bWasClicked;
		});

		if (ClickedResultIt != RecordsView.end())
		{
			for (FDrawnWidgetRecord& Record : RecordsView)
			{
				Record.Widget->State.bClicked = true;
				Record.bClicked = true;
			}
		}

		return RecordsView.front().Widget->State;
	}
}
