#include "UI/NewProjectWindow/NewProjectWindow.h"
#include "UI/MainWindow/ProjectsTab.h"
#include "GUI.h"
#include "Project.h"

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

				const FInputTextBox& ProjectNameInputTextBox = Page.GetInputTextBox("ProjectName");

				FGui::SetCursorPosition(TopOffset);
				FGui::Draw(ProjectNameInputTextBox);
				//FGui::SetCursorPositionX(TopOffset.x);
				//FGui::Draw(CurrentPage.GetInputTextBox("ProjectPath"));

				const FButton& CreateButton = Page.GetButton("Create");
				const ImVec2& CreateButtonSize = FGui::CalculateSize(CreateButton);
				const ImVec2& BottomOffset = Page.BottomOffset;

				if (CreateButton.State.IsClicked())
				{
					std::optional<FProject> Project = NewProject(ProjectNameInputTextBox.Buffer.get());
					if (Project)
					{
						FProjectsTab::GetInstance().bNeedReloadProjects = true;
					}
					Close();
				}

				FGui::SetCursorPosition(FGui::GetWindowSize() - CreateButtonSize - BottomOffset);
				FGui::Draw(CreateButton);
				FGui::UpdateState();
				FGui::DrawTopPanel(Window);
			}
			FGui::EndWindow();
		}
	}

	std::optional<FProject> FNewProjectWindow::NewProject(const std::string& Name,
		const std::filesystem::path& TargetFolderPath)
	{
		using namespace std::filesystem;

		// BSTODO: Cache in Projects.txt ?
		//std::ofstream OutputStream("Projects.txt", std::ios::app);
		//assert(OutputStream.is_open() && "FNewProjectWindow::NewProject: Failed to open output file 'Projects.txt'");
		if (exists(TargetFolderPath))
		{
			if (!exists(absolute(TargetFolderPath.string() + "/" + Name)))
			{
				FProject Project;

				Project.Name = Name;
				Project.Directory = absolute(TargetFolderPath.string() + "/" + Name);
				Project.ContentDirectory = Project.Directory.string() + "/Content";
				Project.StartScenePath = Project.Directory.string() + "/Content/Scenes/Default.bsscene";

				create_directories(Project.StartScenePath.parent_path());

				std::ifstream CMakeInputStream("Resources/Launcher/Generate/Project.txt");
				std::stringstream ProjectBuildFileStringStream;
				ProjectBuildFileStringStream << CMakeInputStream.rdbuf();
				CMakeInputStream.close();
				std::string ProjectBuildFileSource = ProjectBuildFileStringStream.str();

				ProjectBuildFileSource = std::regex_replace(ProjectBuildFileSource,
					std::regex("PLACEHOLDER_PROJECT_NAME"), Project.Name);

				std::string BloodshotMainDirectory = std::filesystem::current_path().parent_path().string();
				std::replace(BloodshotMainDirectory.begin(), BloodshotMainDirectory.end(), '\\', '/');
				ProjectBuildFileSource = std::regex_replace(ProjectBuildFileSource,
					std::regex("PLACEHOLDER_BLOODSHOT_DIR"), BloodshotMainDirectory);

				std::ofstream CMakeOutputStream(Project.Directory.string() + "/CMakeLists.txt");
				CMakeOutputStream << ProjectBuildFileSource;
				CMakeOutputStream.close();

				std::filesystem::create_directories(Project.Directory.string() +
					std::format("/Source/{}/Private", Project.Name));
				std::filesystem::create_directories(Project.Directory.string() +
					std::format("/Source/{}/Public", Project.Name));

				CMakeInputStream.open("Resources/Launcher/Generate/Module.txt");
				CMakeOutputStream.open(Project.Directory.string() +
					std::format("/Source/{}/CMakeLists.txt", Project.Name));
				std::stringstream SStr;
				SStr << CMakeInputStream.rdbuf();
				CMakeInputStream.close();
				std::string ModuleSource = SStr.str();
				ModuleSource = std::regex_replace(ModuleSource,
					std::regex("PLACEHOLDER_PROJECT_NAME"), Project.Name);
				CMakeOutputStream << ModuleSource;
				CMakeOutputStream.close();

				std::ofstream ProjectSourceOutputStream(Project.Directory.string() +
					std::format("/Source/{}/Public/{}.h", Project.Name, Project.Name));
				ProjectSourceOutputStream << "#pragma once\n\n";
				ProjectSourceOutputStream << "#include \"Core.h\"\n";
				ProjectSourceOutputStream.close();
				ProjectSourceOutputStream.open(Project.Directory.string() +
					std::format("/Source/{}/Private/{}.cpp", Project.Name, Project.Name));
				ProjectSourceOutputStream << std::format("#include \"{}.h\"", Project.Name);
				ProjectSourceOutputStream.close();

				std::ofstream DefaultSceneOutputFileStream(Project.StartScenePath);
				DefaultSceneOutputFileStream.close();

				std::ofstream ProjectDescOutputFileStream(Project.Directory.string()
					+ "/"
					+ Name
					+ ".bsproject");

				ProjectDescOutputFileStream << "Name: " << Project.Name << "\n";
				ProjectDescOutputFileStream << "Directory: " << Project.Directory.string() << "\n";
				ProjectDescOutputFileStream << "ContentDirectory: " << Project.ContentDirectory.string() << "\n";
				ProjectDescOutputFileStream << "StartScenePath: " << Project.StartScenePath.string();

				ProjectDescOutputFileStream.close();
				//OutputStream.close();

				return std::make_optional(std::move(Project));
			}
		}

		//OutputStream.close();
		return std::nullopt;
	}

	void FNewProjectWindow::Setup()
	{
		FWindow NewProjectWindow;
		NewProjectWindow.SetTopPanel(FGui::CreateTopPanel([this]() { Close(); }));

		FPage MainPage;
		FInputTextBox ProjectNameInputTextBox;
		ProjectNameInputTextBox.Label = "##ProjectName";
		ProjectNameInputTextBox.Hint = "Project name";
		ProjectNameInputTextBox.Font = FGui::GetFont(18);
		ProjectNameInputTextBox.Buffer = std::make_shared<char[]>(1024);
		ProjectNameInputTextBox.BufferSize = 1024;
		ProjectNameInputTextBox.Color = ImVec4(0.2f, 0.2f, 0.2f, 1.f);
		MainPage.AddInputTextBox("ProjectName", std::move(ProjectNameInputTextBox));

		FButton CreateButton;
		CreateButton.HoveredCursor = ImGuiMouseCursor_Hand;
		CreateButton.Text = "Create";
		CreateButton.Font = ProjectNameInputTextBox.Font;
		CreateButton.TextColor = ImVec4(0.f, 0.f, 0.f, 1.f);
		CreateButton.HoveredTextColor = CreateButton.TextColor;
		CreateButton.Color = (ImVec4)ImColor(239, 216, 0);
		CreateButton.ActiveColor = (ImVec4)ImColor(216, 198, 0);
		CreateButton.HoveredColor = (ImVec4)ImColor(245, 228, 0);
		MainPage.AddButton("Create", std::move(CreateButton));

		NewProjectWindow.SetPage("Main", std::move(MainPage));
		FGui::AddWindow("NewProject", std::move(NewProjectWindow));
	}
}
