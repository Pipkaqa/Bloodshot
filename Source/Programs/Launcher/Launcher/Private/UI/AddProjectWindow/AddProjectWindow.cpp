#include "UI/MainWindow/ProjectsTab.h"
#include "GUI.h"
#include "Misc/IsValidPath.h"
#include "UI/AddProjectWindow/AddProjectWindow.h"

namespace Bloodshot::Launcher
{
	FAddProjectWindow& FAddProjectWindow::GetInstance()
	{
		static FAddProjectWindow Instance;
		return Instance;
	}

	void FAddProjectWindow::Draw()
	{
		if (bOpened)
		{
			FGui::BeginWindow("Add Project", 0, ImVec2(512, 256));
			{
				const FWindow& Window = FGui::GetWindow("AddProject");
				const FPage& Page = Window.GetPage("Main");
				const ImVec2& TopOffset = Page.TopOffset;

				const FInputTextBox& ProjectPathInputTextBox = Page.GetInputTextBox("ProjectPath");
				const bool bPathNotValid = !IsValidPath(ProjectPathInputTextBox.Buffer.get());

				if (bPathNotValid)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.f, 0.f, 1.f));
				}

				FGui::SetCursorPosition(TopOffset);
				FGui::Draw(ProjectPathInputTextBox);

				if (bPathNotValid)
				{
					ImGui::PopStyleColor();
				}

				const FButton& AddButton = Page.GetButton("Add");
				const ImVec2& AddButtonSize = FGui::CalculateSize(AddButton);
				const ImVec2& BottomOffset = Page.BottomOffset;

				if (AddButton.State.IsClicked() && !bPathNotValid)
				{
					AddProject(ProjectPathInputTextBox.Buffer.get());
				}

				FGui::SetCursorPosition(FGui::GetWindowSize() - AddButtonSize - BottomOffset);
				FGui::Draw(AddButton);
				FGui::UpdateState();
				FGui::DrawTopPanel(Window);
			}
			FGui::EndWindow();
		}
	}

	void FAddProjectWindow::AddProject(const char* InDirectory)
	{
		if (std::filesystem::exists(InDirectory))
		{
			FProjectsTab& ProjectsTab = FProjectsTab::GetInstance();
			Shared::FProject Project;
			const bool bLoaded = Shared::FProject::TryLoadFromDirectory(Project, InDirectory);
			if (bLoaded)
			{
				Shared::FProjectArchive& Archive = ProjectsTab.ProjectArchive;
				if (!Archive.Contains(Project))
				{
					Archive.Add(Project);
					ProjectsTab.bNeedReloadProjects = true;
					Close();
				}
			}
		}
	}

	void FAddProjectWindow::Setup()
	{
		FWindow AddProjectWindow;
		AddProjectWindow.SetTopPanel(FGui::CreateTopPanel([this]() { Close(); }));

		FPage MainPage;
		FInputTextBox ProjectPathInputTextBox;
		ProjectPathInputTextBox.Label = "##ProjectPath";
		ProjectPathInputTextBox.Hint = "Project Path";
		ProjectPathInputTextBox.Font = FGui::GetFont(18);
		ProjectPathInputTextBox.Buffer = std::make_shared<char[]>(1024);
		ProjectPathInputTextBox.BufferSize = 1024;
		ProjectPathInputTextBox.Color = ImVec4(0.2f, 0.2f, 0.2f, 1.f);
		MainPage.AddInputTextBox("ProjectPath", std::move(ProjectPathInputTextBox));

		FButton CreateButton;
		CreateButton.HoveredCursor = ImGuiMouseCursor_Hand;
		CreateButton.Text = "Add";
		CreateButton.Font = FGui::GetFont(18);
		CreateButton.TextColor = ImVec4(0.f, 0.f, 0.f, 1.f);
		CreateButton.HoveredTextColor = CreateButton.TextColor;
		CreateButton.Color = (ImVec4)ImColor(239, 216, 0);
		CreateButton.ActiveColor = (ImVec4)ImColor(216, 198, 0);
		CreateButton.HoveredColor = (ImVec4)ImColor(245, 228, 0);
		MainPage.AddButton("Add", std::move(CreateButton));

		AddProjectWindow.SetPage("Main", std::move(MainPage));
		FGui::AddWindow("AddProject", std::move(AddProjectWindow));
	}

	void FAddProjectWindow::ResetInputTextBoxes()
	{
		const FWindow& Window = FGui::GetWindow("AddProject");
		const FPage& Page = Window.GetPage("Main");
		const FInputTextBox& ProjectPathInputTextBox = Page.GetInputTextBox("ProjectPath");
		strcpy(ProjectPathInputTextBox.Buffer.get(), "");
	}
}
