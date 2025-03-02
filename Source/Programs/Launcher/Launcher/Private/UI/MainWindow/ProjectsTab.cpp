#include "UI/MainWindow/ProjectsTab.h"
#include "Settings.h"

#include <fstream>
#include <regex>

namespace Bloodshot::Launcher
{
	FProjectsTab& FProjectsTab::GetInstance()
	{
		static FProjectsTab Instance;
		return Instance;
	}

	void FProjectsTab::Draw()
	{
		UpdateProjects(FSettings::GetProjectsFolderPath());

		FWindow& MainWindow = FGui::GetWindow("Main");
		FPage& MainPage = MainWindow.GetPage("Main");

		const FWidgetGroup& ProjectButtonGroup = MainPage.GetWidgetGroup("Projects");

		const ImVec2& BottomOffset = MainPage.BottomOffset;
		const ImVec2& CornerProjectListPosition = FGui::GetCursorPosition();

		const ImVec2& FullTextSize = FGui::CalculateSize(MainPage.GetText("_ProjectButtonNameTextDummy"))
			+ FGui::CalculateSize(MainPage.GetText("_ProjectButtonPathTextDummy"));
		const ImVec2& ButtonSize = ImVec2(FGui::GetWindowSize().x - CornerProjectListPosition.x,
			FullTextSize.y + FGui::GetFramePadding().y * 2.f);

		const FButton& NewProjectButton = MainPage.GetButton("NewProject");
		const ImVec2& NewProjectButtonSize = FGui::CalculateSize(NewProjectButton);

		FGui::MoveCursorX(ButtonSize.x - NewProjectButtonSize.x - BottomOffset.x);
		FGui::MoveCursorY(ButtonSize.y / 2.f - NewProjectButtonSize.y / 2.f);

		const ImVec2& NewProjectButtonLocalPosition = FGui::GetCursorPosition();

		FGui::Draw(NewProjectButton, NewProjectButtonSize);

		const FButton& AddProjectButton = MainPage.GetButton("AddProject");
		const ImVec2& AddProjectButtonSize = FGui::CalculateSize(AddProjectButton);
		const ImVec2& AddProjectButtonExtendedSize = ImVec2(AddProjectButtonSize.x * 2.f, AddProjectButtonSize.y);

		FGui::SetCursorPosition(NewProjectButtonLocalPosition);
		FGui::MoveCursorX(-AddProjectButtonExtendedSize.x - 4.f);
		FGui::SetCursorPositionY(CornerProjectListPosition.y);
		FGui::MoveCursorY(ButtonSize.y / 2.f - AddProjectButtonExtendedSize.y / 2.f);
		FGui::Draw(AddProjectButton, AddProjectButtonExtendedSize);

		if (!ProjectButtonGroup.GetSize()) return;

		FGui::SetCursorPosition(CornerProjectListPosition);
		FGui::MoveCursorY(ButtonSize.y);

		const ImVec2& ProjectsSubWindowSize = ImVec2(ButtonSize.x,
			FGui::GetWindowSize().y - CornerProjectListPosition.y - ButtonSize.y);
		ImGui::BeginChild("ProjectsSubWindow", ProjectsSubWindowSize);
		{
			for (size_t i = 0; i < Projects.size(); ++i)
			{
				const std::string& CountString = std::to_string(i);

				const FText& NameText = ProjectButtonGroup.GetText("Name" + CountString);
				const FText& PathText = ProjectButtonGroup.GetText("Path" + CountString);

				const ImVec2& FramePadding = FGui::GetFramePadding();
				const ImVec2& NameTextSize = FGui::CalculateSize(NameText);
				const ImVec2& FullTextSize = NameTextSize + FGui::CalculateSize(PathText);
				const ImVec2& CursorLocalPosition = FGui::GetCursorPosition();

				FGui::Draw(ProjectButtonGroup.GetButton(CountString), ButtonSize);

				const ImVec2& ButtonLocalPosition = ImVec2(CursorLocalPosition.x + 16.f, CursorLocalPosition.y);
				const ImVec2& TextPosition = ImVec2(ButtonLocalPosition.x,
					ButtonLocalPosition.y + ButtonSize.y - FullTextSize.y - FramePadding.y);

				FGui::SetCursorPosition(TextPosition);
				FGui::Draw(NameText);

				FGui::SetCursorPosition(TextPosition);
				FGui::MoveCursorX(1.f);
				FGui::MoveCursorY(NameTextSize.y);
				FGui::Draw(PathText);
				FGui::UpdateState(3);

				FGui::SetCursorPosition(CursorLocalPosition);
				FGui::MoveCursorY(ButtonSize.y);
			}
		}
		ImGui::EndChild();
	}

	void FProjectsTab::UpdateProjects(const std::filesystem::path& TargetFolderPath)
	{
		using namespace std::filesystem;
		FWindow& MainWindow = FGui::GetWindow("Main");
		FPage& MainPage = MainWindow.GetPage("Main");
		if (TargetFolderPath.empty() || !exists(TargetFolderPath))
		{
			create_directory(TargetFolderPath);
		}
		Projects.clear();

		ProjectArchive.ForEach([this](const Shared::FProject& InProject)
		{
			Projects.emplace_back(InProject);
		});

		FWidgetGroup ProjectButtonGroup;
		size_t ProjectCount = 0;

		ImFont* NameTextFont = FGui::GetFont(18);
		ImFont* PathTextFont = FGui::GetFont(15);

		const ImVec4& ButtonHoveredColor = ImVec4(0.25f, 0.25f, 0.25f, 1.f);
		const ImVec4& TextColor = ImVec4(0.85f, 0.85f, 0.85f, 1.f);
		const ImVec4& HoveredTextColor = ImVec4(1.f, 1.f, 1.f, 1.f);

		for (const Shared::FProject& Project : Projects)
		{
			const std::string& Name = Project.Name;
			const std::string& Directory = Project.Directory;
			const std::string& CountString = std::to_string(ProjectCount);

			FButton Button;
			Button.HoveredCursor = ImGuiMouseCursor_Hand;
			Button.Text = "##Project" + Name + CountString;
			Button.Font = NameTextFont;
			Button.HoveredColor = ButtonHoveredColor;
			Button.OnClickEvent = [this, &Project]() 
			{ 
				IProcess::Create(std::format("{} {}",
					"Bloodshot Engine.exe", "Project:" + Project.Directory).c_str());
			};
			ProjectButtonGroup.AddButton(std::string(CountString), std::move(Button));

			FText NameText;
			NameText.HoveredCursor = ImGuiMouseCursor_Hand;
			NameText.Value = Name;
			NameText.Font = Button.Font;
			NameText.Color = TextColor;
			NameText.HoveredColor = HoveredTextColor;
			ProjectButtonGroup.AddText("Name" + std::string(CountString), std::move(NameText));

			FText PathText;
			PathText.HoveredCursor = ImGuiMouseCursor_Hand;
			PathText.Value = Directory;
			PathText.Font = PathTextFont;
			PathText.Color = TextColor;
			PathText.HoveredColor = HoveredTextColor;
			ProjectButtonGroup.AddText("Path" + std::string(CountString), std::move(PathText));

			++ProjectCount;
		}

		MainPage.SetWidgetGroup("Projects", std::move(ProjectButtonGroup));
		bNeedReloadProjects = false;
	}
}
