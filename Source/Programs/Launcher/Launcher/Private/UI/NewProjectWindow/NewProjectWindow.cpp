#include "UI/NewProjectWindow/NewProjectWindow.h"
#include "GUI.h"
#include "Misc/IsValidPath.h"
#include "Project.h"
#include "Settings.h"
#include "UI/MainWindow/ProjectsTab.h"

#include <fstream>
#include <regex>

namespace Bloodshot::Launcher
{
	FNewProjectWindow& FNewProjectWindow::GetInstance()
	{
		static FNewProjectWindow Instance;
		return Instance;
	}

	void FNewProjectWindow::Draw()
	{
		if (bOpened)
		{
			FGui::BeginWindow("New Project", 0, ImVec2(512, 256));
			{
				const FWindow& Window = FGui::GetWindow("NewProject");
				const FPage& Page = Window.GetPage("Main");
				const ImVec2& TopOffset = Page.TopOffset;

				bool bValidInput = true;

				const FInputTextBox& ProjectNameInputTextBox = Page.GetInputTextBox("ProjectName");
				{
					const bool bPathNotValid = !IsValidPath(ProjectNameInputTextBox.Buffer.get());

					if (bPathNotValid)
					{
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.f, 0.f, 1.f));
					}

					FGui::SetCursorPosition(TopOffset);
					FGui::Draw(ProjectNameInputTextBox);

					if (bPathNotValid)
					{
						ImGui::PopStyleColor();
					}
					bValidInput = bValidInput && !bPathNotValid;
				}

				const FInputTextBox& ProjectPathInputTextBox = Page.GetInputTextBox("ProjectPath");
				{
					const bool bPathNotValid = !IsValidPath(ProjectPathInputTextBox.Buffer.get());

					if (bPathNotValid)
					{
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.f, 0.f, 1.f));
					}

					FGui::SetCursorPositionX(TopOffset.x);
					FGui::Draw(ProjectPathInputTextBox);

					if (bPathNotValid)
					{
						ImGui::PopStyleColor();
					}
					bValidInput = bValidInput && !bPathNotValid;
				}

				const FButton& CreateButton = Page.GetButton("Create");
				const ImVec2& CreateButtonSize = FGui::CalculateSize(CreateButton);
				const ImVec2& BottomOffset = Page.BottomOffset;

				if (CreateButton.State.IsClicked() && bValidInput)
				{
					if (NewProject(ProjectNameInputTextBox.Buffer.get(), ProjectPathInputTextBox.Buffer.get()))
					{
						FProjectsTab::GetInstance().bNeedReloadProjects = true;
						Close();
					}
				}

				FGui::SetCursorPosition(FGui::GetWindowSize() - CreateButtonSize - BottomOffset);
				FGui::Draw(CreateButton);
				FGui::UpdateState();
				FGui::DrawTopPanel(Window);
			}
			FGui::EndWindow();
		}
	}

	void FNewProjectWindow::Setup()
	{
		FWindow NewProjectWindow;
		NewProjectWindow.SetTopPanel(FGui::CreateTopPanel([this]() { Close(); }));

		FPage MainPage;
		FInputTextBox ProjectNameInputTextBox;
		ProjectNameInputTextBox.Label = "##ProjectName";
		ProjectNameInputTextBox.Hint = "Project Name";
		ProjectNameInputTextBox.Font = FGui::GetFont(18);
		ProjectNameInputTextBox.Buffer = std::make_shared<char[]>(1024);
		strcpy(ProjectNameInputTextBox.Buffer.get(), "New Project");
		ProjectNameInputTextBox.BufferSize = 1024;
		ProjectNameInputTextBox.Color = ImVec4(0.2f, 0.2f, 0.2f, 1.f);
		MainPage.AddInputTextBox("ProjectName", std::move(ProjectNameInputTextBox));

		FButton CreateButton;
		CreateButton.HoveredCursor = ImGuiMouseCursor_Hand;
		CreateButton.Text = "Create";
		CreateButton.Font = FGui::GetFont(18);
		CreateButton.TextColor = ImVec4(0.f, 0.f, 0.f, 1.f);
		CreateButton.HoveredTextColor = CreateButton.TextColor;
		CreateButton.Color = (ImVec4)ImColor(239, 216, 0);
		CreateButton.ActiveColor = (ImVec4)ImColor(216, 198, 0);
		CreateButton.HoveredColor = (ImVec4)ImColor(245, 228, 0);
		MainPage.AddButton("Create", std::move(CreateButton));

		FInputTextBox ProjectPathInputTextBox;
		ProjectPathInputTextBox.Label = "##ProjectPath";
		ProjectPathInputTextBox.Hint = "Project Path";
		ProjectPathInputTextBox.Font = FGui::GetFont(18);
		ProjectPathInputTextBox.Buffer = std::make_shared<char[]>(1024);
		strcpy(ProjectPathInputTextBox.Buffer.get(), FSettings::GetProjectsFolderPath().c_str());
		ProjectPathInputTextBox.BufferSize = 1024;
		ProjectPathInputTextBox.Color = ImVec4(0.2f, 0.2f, 0.2f, 1.f);
		MainPage.AddInputTextBox("ProjectPath", std::move(ProjectPathInputTextBox));

		NewProjectWindow.SetPage("Main", std::move(MainPage));
		FGui::AddWindow("NewProject", std::move(NewProjectWindow));
	}

	bool FNewProjectWindow::NewProject(const std::string& Name, const std::filesystem::path& TargetFolderPath)
	{
		using namespace std::filesystem;
		if (!exists(TargetFolderPath))
		{
			create_directories(TargetFolderPath);
		}

		Shared::FProject Project;
		Project.Name = Name;
		Project.Directory = TargetFolderPath.string() + "/" + Name;
		Project.ContentDirectory = Project.Directory + "/Content";
		Project.EditorStartupScene = Project.Directory + "/Content/Scenes/Default.bsscene";
		Project.GameDefaultScene = Project.EditorStartupScene;

		if (FProjectsTab::GetInstance().ProjectArchive.Contains(Project))
		{
			return false;
		}

		create_directories(Project.Directory + "/Content/Scenes");

		std::ifstream InputStream("Resources/Launcher/Generate/Project.txt");
		std::string StreamContent = (std::stringstream() << InputStream.rdbuf()).str();
		InputStream.close();

		StreamContent = std::regex_replace(StreamContent,
			std::regex("PLACEHOLDER_PROJECT_NAME"), Project.Name);

		{
			std::string BloodshotMainDir = std::filesystem::current_path().parent_path().string();
			std::replace(BloodshotMainDir.begin(), BloodshotMainDir.end(), '\\', '/');
			StreamContent = std::regex_replace(StreamContent,
				std::regex("PLACEHOLDER_BLOODSHOT_DIR"), BloodshotMainDir);
		}

		std::ofstream OutputStream(Project.Directory + "/CMakeLists.txt");
		OutputStream << StreamContent;
		OutputStream.close();

		std::filesystem::create_directories(Project.Directory +
			std::format("/Source/{}/Private", Project.Name));
		std::filesystem::create_directories(Project.Directory +
			std::format("/Source/{}/Public", Project.Name));

		InputStream.open("Resources/Launcher/Generate/Module.txt");
		OutputStream.open(Project.Directory +
			std::format("/Source/{}/CMakeLists.txt", Project.Name));
		StreamContent = (std::stringstream() << InputStream.rdbuf()).str();
		InputStream.close();
		StreamContent = std::regex_replace(StreamContent,
			std::regex("PLACEHOLDER_PROJECT_NAME"), Project.Name);
		OutputStream << StreamContent;
		OutputStream.close();

		OutputStream.open(Project.Directory +
			std::format("/Source/{}/Public/{}.h", Project.Name, Project.Name));
		OutputStream << "#pragma once\n\n";
		OutputStream << "#include \"Core.h\"\n";
		OutputStream.close();
		OutputStream.open(Project.Directory +
			std::format("/Source/{}/Private/{}.cpp", Project.Name, Project.Name));
		OutputStream << std::format("#include \"{}.h\"", Project.Name);
		OutputStream.close();

		Project.SaveToFile(Project.Directory
			+ "/"
			+ Name
			+ ".bsproject");

		FProjectsTab::GetInstance().ProjectArchive.Add(Project);
		return true;
	}

	void FNewProjectWindow::ResetInputTextBoxes()
	{
		const FWindow& Window = FGui::GetWindow("NewProject");
		const FPage& Page = Window.GetPage("Main");

		const FInputTextBox& ProjectNameInputTextBox = Page.GetInputTextBox("ProjectName");
		const FInputTextBox& ProjectPathInputTextBox = Page.GetInputTextBox("ProjectPath");

		strcpy(ProjectNameInputTextBox.Buffer.get(), "New Project");
		strcpy(ProjectPathInputTextBox.Buffer.get(), FSettings::GetProjectsFolderPath().c_str());
	}
}
