#include "Launcher.h"
#include "Platform/Process.h"

#include <fstream>

namespace Bloodshot::Launcher
{
	FLauncher::FLauncher()
	{
		Init();
	}

	FLauncher::~FLauncher()
	{
		SaveSettings();
	}

	void FLauncher::Init()
	{
		LoadFonts();
		LoadTextures();

		CreateMainPage();
		CreateSettingsPage();
		CreateAddProjectPage();
		CreateNewProjectPage();
		CreateOpenProjectPage();

		LoadSettings();

		Run();
	}

	void FLauncher::Run()
	{
		ImVec2& LocalMousePosition = DebugInfo.LocalMousePosition;
		bool& bDebugMode = DebugInfo.bDebugMode;

		while (FGui::IsRunning())
		{
			FGui::BeginRender();
			bDebugMode ? FGui::EnableDebugMode() : void();
			{
				FGui::BeginWindow("Bloodshot Engine Launcher",
					ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse,
					IConstants::MinWindowSize);
				{
					LocalMousePosition = FGui::GetRelativeMousePosition();
					FGui::DrawPage(MainWindowSpec.PageTypeToIDMap[MainWindowSpec.CurrentPage]);
				}
				FGui::EndWindow();

				if (bAddProjectWindowOpened && 0)
				{
					FGui::BeginWindow("Add Project", 0, ImVec2(512, 256));
					{
						//FGui::DrawPage("AddProject");
					}
					FGui::EndWindow();
				}

				if (bNewProjectWindowOpened)
				{
					FGui::BeginWindow("New Project", 0, ImVec2(512, 256));
					{
						FGui::DrawPage(NewProjectWindowSpec.PageTypeToIDMap[NewProjectWindowSpec.CurrentPage]);
					}
					FGui::EndWindow();
				}

				if (bOpenProjectWindowOpened)
				{
					FGui::BeginWindow("Open Project", 0, ImVec2(512, 256));
					{
						//FGui::DrawPage("OpenProject");
					}
					FGui::EndWindow();
				}

				const std::list<FDrawnWidgetRecord>& DrawnWidgetRecords = FGui::GetDrawnWidgetRecordStack();

				FGui::BeginWindow("Debug Info", 0, ImVec2(256, 128));
				{
					ImGui::Text(std::format("Mouse Position: X:{} Y:{}", LocalMousePosition.x, LocalMousePosition.y).c_str());
					ImGui::Checkbox("Debug Mode", &bDebugMode);

					ImGui::Dummy(ImVec2(0.f, 4.f));
					ImGui::Text("Widgets Draw Stack");

					if (ImGui::BeginTable("##WidgetsDrawStackTable", 6, ImGuiTableFlags_Borders))
					{
						size_t MaxTypeLength = 0;
						size_t MaxInfoLength = 0;

						for (const FDrawnWidgetRecord& Record : DrawnWidgetRecords)
						{
							const size_t TypeLength = Record.Type.length();
							const size_t InfoLength = Record.Info.length();

							if (MaxTypeLength < TypeLength) MaxTypeLength = TypeLength;
							if (MaxInfoLength < InfoLength) MaxInfoLength = InfoLength;
						}

						ImGui::TableSetupColumn("UniqueID", ImGuiTableColumnFlags_WidthFixed,
							ImGui::CalcTextSize(std::format("{}", std::numeric_limits<int>::max()).c_str()).x);
						ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed,
							(float)ImGui::CalcTextSize(" ").x * MaxTypeLength);
						ImGui::TableSetupColumn("Info", ImGuiTableColumnFlags_WidthFixed,
							(float)ImGui::CalcTextSize(" ").x * MaxInfoLength);
						ImGui::TableSetupColumn("Position", ImGuiTableColumnFlags_WidthStretch);
						ImGui::TableSetupColumn("Hovered", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("Hovered").x);
						ImGui::TableSetupColumn("Clicked", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("Clicked").x);
						ImGui::TableHeadersRow();

						for (const FDrawnWidgetRecord& Record : DrawnWidgetRecords)
						{
							ImGui::TableNextColumn();
							ImGui::Text(std::format("{}", Record.UniqueID).c_str());
							ImGui::TableNextColumn();
							ImGui::Text(std::format("{}", Record.Type).c_str());
							ImGui::TableNextColumn();
							ImGui::Text(std::format("{}", Record.Info).c_str());
							ImGui::TableNextColumn();

							const ImVec2& WidgetPosition = Record.Position;

							ImGui::Text(std::format("{}; {}", WidgetPosition.x, WidgetPosition.y).c_str());
							ImGui::TableNextColumn();
							ImGui::Text(std::format("{}", Record.bHovered).c_str());
							ImGui::TableNextColumn();
							ImGui::Text(std::format("{}", Record.bClicked).c_str());
						}
						ImGui::EndTable();
					}
				}
				FGui::EndWindow();
			}
			FGui::DisableDebugMode();
			FGui::EndRender();
		}
	}

	void FLauncher::LoadSettings()
	{
		const bool bSettingsFound = std::filesystem::exists(Settings.Filename);

		if (!bSettingsFound)
		{
			strcpy_s(Settings.ProjectsFolderPath.get(), Settings.ProjectsFolderPathSize, "Projects");
		}
		else
		{
			std::ifstream InputFileStream(Settings.Filename);
			std::string Line;

			auto ReadValue = [&InputFileStream, &Line](char* Out, std::function<void(char*, const std::vector<std::string>&)> HowToRead)
				{
					std::getline(InputFileStream, Line);
					const std::vector<std::string>& Words = Split(Line, " ");
					HowToRead(Out, Words);
				};

			ReadValue(Settings.ProjectsFolderPath.get(),
				[this](char* Out, const std::vector<std::string>& Words)
				{
					if (Words.size() > 1) strcpy_s(Out, Settings.ProjectsFolderPathSize, Words[1].c_str());
				});

			InputFileStream.close();
		}

		const FPage& SettingsPage = FGui::GetPage("Settings");
		const FInputTextBox& ProjectsPathInputTextBox = SettingsPage.GetInputTextBox("ProjectsPath");

		strcpy_s(ProjectsPathInputTextBox.Buffer.get(), ProjectsPathInputTextBox.BufferSize, Settings.ProjectsFolderPath.get());
	}

	void FLauncher::SaveSettings()
	{
		std::ofstream OutputFileStream(Settings.Filename, std::ios::out | std::ios::trunc);
		OutputFileStream << "ProjectsFolderPath: " << Settings.ProjectsFolderPath.get();
		OutputFileStream.close();
	}

	void FLauncher::AddProject()
	{

	}

	void FLauncher::NewProject(const std::filesystem::path& Path)
	{
		using namespace std::filesystem;

		std::ofstream OutputFileStream("Projects.txt", std::ios::app);

		if (!OutputFileStream.is_open())
		{
			std::terminate();
		}

		if (Path.empty())
		{
			const path& ProjectsPath = path(Settings.ProjectsFolderPath.get());

			if (exists(ProjectsPath))
			{
				if (!exists(absolute(ProjectsPath.string() + "\\" + NewProjectNameHolder)))
				{
					FProject Project;

					Project.Name = NewProjectNameHolder;
					Project.Directory = absolute(ProjectsPath.string() + "\\" + NewProjectNameHolder);
					Project.ContentDirectory = Project.Directory.string() + "\\Content";
					Project.StartScenePath = Project.Directory.string() + "\\Content\\Scenes\\Default.bsscene";

					create_directories(Project.StartScenePath.parent_path());

					std::ofstream CMakeOutputFileStream(Project.Directory.string() + "\\CMakeLists.txt");
					CMakeOutputFileStream.close();

					std::ofstream DefaultSceneOutputFileStream(Project.StartScenePath);
					DefaultSceneOutputFileStream.close();

					std::ofstream ProjectDescOutputFileStream(Project.Directory.string()
						+ "\\"
						+ NewProjectNameHolder
						+ ".bsproject");

					ProjectDescOutputFileStream << "Name: " << Project.Name << "\n";
					ProjectDescOutputFileStream << "Directory: " << Project.Directory.string() << "\n";
					ProjectDescOutputFileStream << "ContentDirectory: " << Project.ContentDirectory.string() << "\n";
					ProjectDescOutputFileStream << "StartScenePath: " << Project.StartScenePath.string();

					ProjectList.emplace_back(std::move(Project));

					ProjectDescOutputFileStream.close();
				}
			}
		}

		OutputFileStream.close();
		bNeedReloadProjects = true;
	}

	void FLauncher::OpenProject(const FProject& Project)
	{
		IProcess::Create(std::format("{} {}\\{}.bsproject", "Editor.exe", Project.Directory.string(), Project.Name).c_str());
	}

	void FLauncher::LoadFonts()
	{
		for (size_t FontSize = 8; FontSize <= 22; ++FontSize)
		{
			FGui::LoadFontFromFile(IConstants::FontPath.string().c_str(), (float)FontSize);
		}
	}

	void FLauncher::LoadTextures()
	{
		FGui::LoadTextureFromFile("Logo512", IConstants::LogoPath.string().c_str());
		FGui::LoadTextureFromFile("SettingsIcon512", IConstants::SettingsIconPath.string().c_str());
		FGui::LoadTextureFromFile("BackIcon512", IConstants::BackIconPath.string().c_str());
	}

	void FLauncher::UpdateProjectList()
	{
		if (!bNeedReloadProjects) return;

		using namespace std::filesystem;

		const path& ProjectsFolderPath = path(Settings.ProjectsFolderPath.get());

		if (ProjectsFolderPath.empty())
		{
			FGui::GetPage(MainWindowSpec.PageTypeToIDMap[MainWindowSpec.CurrentPage]).SetWidgetGroup("ProjectList", FWidgetGroup());
			return;
		}

		if (!exists(ProjectsFolderPath))
		{
			create_directory(ProjectsFolderPath);
		}

		ProjectList.clear();

		for (const directory_entry& ProjectDirectory : directory_iterator(ProjectsFolderPath))
		{
			FProject Project;

			for (const directory_entry& ProjectRootFiles : directory_iterator(ProjectDirectory))
			{
				const path& ProjectRootFilesPath = ProjectRootFiles.path();

				if (ProjectRootFilesPath.extension() != ".bsproject") continue;

				std::ifstream InputFileStream(ProjectRootFilesPath.string());
				std::string Line;

				auto ReadValue = [&InputFileStream, &Line](auto& Out)
					{
						std::getline(InputFileStream, Line);
						const std::vector<std::string>& Words = Split(Line, " ");
						if (Words.size() > 1) Out = Words[1];
					};

				ReadValue(Project.Name);
				ReadValue(Project.Directory);
				ReadValue(Project.ContentDirectory);
				ReadValue(Project.StartScenePath);

				ProjectList.emplace_back(std::move(Project));
				break;
			}
		}

		FPage& CurrentPage = FGui::GetPage(MainWindowSpec.PageTypeToIDMap[MainWindowSpec.CurrentPage]);

		FWidgetGroup ProjectButtonGroup;

		size_t ProjectCount = 0;

		ImFont* NameTextFont = FGui::GetFont(18);
		ImFont* PathTextFont = FGui::GetFont(15);

		const ImVec4& ButtonHoveredColor = ImVec4(0.25f, 0.25f, 0.25f, 1.f);
		const ImVec4& TextColor = ImVec4(0.85f, 0.85f, 0.85f, 1.f);
		const ImVec4& HoveredTextColor = ImVec4(1.f, 1.f, 1.f, 1.f);

		for (const FProject& Project : ProjectList)
		{
			const std::string& Name = Project.Name;
			const std::string& Directory = Project.Directory.string();
			const std::string& ContentDirectory = Project.ContentDirectory.string();
			const std::string& StartSceneDirectory = Project.StartScenePath.string();
			const std::string& CountString = std::to_string(ProjectCount);

			FButton Button;
			Button.HoveredCursor = ImGuiMouseCursor_Hand;
			Button.Text = "##Project" + Name + CountString;
			Button.Font = NameTextFont;
			Button.HoveredColor = ButtonHoveredColor;
			Button.OnClickEvent = [this, &Project]() { OpenProject(Project); };
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

		CurrentPage.SetWidgetGroup("ProjectList", std::move(ProjectButtonGroup));

		bNeedReloadProjects = false;
	}

	FTopPanel FLauncher::CreateTopPanel(std::function<void()>&& CloseButtonFunc,
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

	void FLauncher::CreateMainPage()
	{
		FPage MainPage;

		MainPage.SetTopPanel(CreateTopPanel([]() { FGui::Shutdown(); }));

		FImageButton LogoImage;
		LogoImage.HoveredCursor = ImGuiMouseCursor_Hand;
		LogoImage.UniqueID = "##LogoImageButton";
		LogoImage.Texture = FGui::GetTexture("Logo512");
		LogoImage.Size = ImVec2((float)LogoImage.Texture.Width, (float)LogoImage.Texture.Height) / 7;
		LogoImage.ImageColor = ImVec4(1.f, 1.f, 1.f, 1.f);
		LogoImage.HoveredImageColor = LogoImage.ImageColor;
		LogoImage.ButtonColor = ImVec4();
		LogoImage.ActiveButtonColor = LogoImage.ButtonColor;
		LogoImage.HoveredButtonColor = LogoImage.ButtonColor;
		LogoImage.OnClickEvent = []()
			{
				ImGui::GetPlatformIO().Platform_OpenInShellFn(ImGui::GetCurrentContext(), "https://github.com/Pipkaqa/Bloodshot");
			};

		FButton GithubLinkButton;
		GithubLinkButton.HoveredCursor = ImGuiMouseCursor_Hand;
		GithubLinkButton.Text = "Bloodshot Engine";
		GithubLinkButton.Font = FGui::GetFont(22);
		GithubLinkButton.TextColor = ImVec4(1.f, 1.f, 1.f, 1.f);
		GithubLinkButton.HoveredTextColor = GithubLinkButton.TextColor;
		GithubLinkButton.Color = ImVec4(0.f, 0.f, 0.f, 0.f);
		GithubLinkButton.ActiveColor = GithubLinkButton.ActiveColor;
		GithubLinkButton.OnClickEvent = LogoImage.OnClickEvent;
		MainPage.AddImageButton("Logo", std::move(LogoImage));
		MainPage.AddButton("GithubLink", std::move(GithubLinkButton));

		FButton ProjectsButton;
		ProjectsButton.HoveredCursor = ImGuiMouseCursor_Hand;
		ProjectsButton.Text = "Projects";
		ProjectsButton.Font = GithubLinkButton.Font;
		ProjectsButton.TextColor = ImVec4(0.75f, 0.75f, 0.75f, 1.f);
		ProjectsButton.HoveredTextColor = ImVec4(1.f, 1.f, 1.f, 1.f);
		ProjectsButton.Color = ImVec4();
		ProjectsButton.ActiveColor = ImVec4();
		ProjectsButton.HoveredColor = ImVec4(0.25f, 0.25f, 0.25f, 1.f);
		ProjectsButton.OnClickEvent = [this]()
			{
				MainWindowSpec.MainPageSpec.CurrentTab = FMainWindowSpec::FMainPageSpec::ETabType::Projects;
			};
		MainPage.AddButton("Projects", std::move(ProjectsButton));

		FLine HorizontalMainSeparator;
		HorizontalMainSeparator.Color = ImVec4(0.25f, 0.25f, 0.25f, 1.f);
		HorizontalMainSeparator.Thickness = 0.45f;
		MainPage.AddLine("HorizontalMainSeparator", std::move(HorizontalMainSeparator));

		FLine VerticalMainSeparator;
		VerticalMainSeparator.Color = ImVec4(0.25f, 0.25f, 0.25f, 1.f);
		VerticalMainSeparator.Thickness = 0.95f;
		MainPage.AddLine("VerticalMainSeparator", std::move(VerticalMainSeparator));

		FButton ChangelogButton;
		ChangelogButton.HoveredCursor = ImGuiMouseCursor_Hand;
		ChangelogButton.Text = "Changelog";
		ChangelogButton.Font = GithubLinkButton.Font;
		ChangelogButton.TextColor = ImVec4(0.75f, 0.75f, 0.75f, 1.f);
		ChangelogButton.HoveredTextColor = ImVec4(1.f, 1.f, 1.f, 1.f);
		ChangelogButton.Color = ImVec4();
		ChangelogButton.ActiveColor = ImVec4();
		ChangelogButton.HoveredColor = ImVec4(0.25f, 0.25f, 0.25f, 1.f);
		ChangelogButton.OnClickEvent = [this]()
			{
				MainWindowSpec.MainPageSpec.CurrentTab = FMainWindowSpec::FMainPageSpec::ETabType::Changelog;
			};
		MainPage.AddButton("Changelog", std::move(ChangelogButton));

		FButton AddProjectButton;
		AddProjectButton.HoveredCursor = ImGuiMouseCursor_Hand;
		AddProjectButton.Text = "Add";
		AddProjectButton.Font = FGui::GetFont(18);
		AddProjectButton.TextColor = ImVec4(0.f, 0.f, 0.f, 1.f);
		AddProjectButton.HoveredTextColor = AddProjectButton.TextColor;
		AddProjectButton.Color = (ImVec4)ImColor(239, 216, 0);
		AddProjectButton.ActiveColor = (ImVec4)ImColor(216, 198, 0);
		AddProjectButton.HoveredColor = (ImVec4)ImColor(245, 228, 0);
		AddProjectButton.OnClickEvent = [this]() { bAddProjectWindowOpened = true; };
		MainPage.AddButton("AddProject", std::move(AddProjectButton));

		FButton NewProjectButton;
		NewProjectButton.HoveredCursor = ImGuiMouseCursor_Hand;
		NewProjectButton.Text = "New project";
		NewProjectButton.Font = AddProjectButton.Font;
		NewProjectButton.TextColor = ImVec4(0.f, 0.f, 0.f, 1.f);
		NewProjectButton.HoveredTextColor = NewProjectButton.TextColor;
		NewProjectButton.Color = (ImVec4)ImColor(239, 216, 0);
		NewProjectButton.ActiveColor = (ImVec4)ImColor(216, 198, 0);
		NewProjectButton.HoveredColor = (ImVec4)ImColor(245, 228, 0);
		NewProjectButton.OnClickEvent = [this]() { bNewProjectWindowOpened = true; };
		MainPage.AddButton("NewProject", std::move(NewProjectButton));

		FText ProjectButtonNameTextDummy;
		ProjectButtonNameTextDummy.Font = FGui::GetFont(18);
		MainPage.AddText("_ProjectButtonNameTextDummy", std::move(ProjectButtonNameTextDummy));

		FText ProjectButtonPathTextDummy;
		ProjectButtonPathTextDummy.Font = FGui::GetFont(15);
		MainPage.AddText("_ProjectButtonPathTextDummy", std::move(ProjectButtonPathTextDummy));

		FButton LaunchButton;
		LaunchButton.HoveredCursor = ImGuiMouseCursor_Hand;
		LaunchButton.Text = "Launch";
		LaunchButton.Font = AddProjectButton.Font;
		LaunchButton.TextColor = ImVec4(0.f, 0.f, 0.f, 1.f);
		LaunchButton.HoveredTextColor = LaunchButton.TextColor;
		LaunchButton.Color = (ImVec4)ImColor(239, 216, 0);
		LaunchButton.ActiveColor = (ImVec4)ImColor(216, 198, 0);
		LaunchButton.HoveredColor = (ImVec4)ImColor(245, 228, 0);
		LaunchButton.OnClickEvent = []() {}; // BSTODO: Change
		MainPage.AddButton("Launch", std::move(LaunchButton));

		FImageButton SettingsImageButton;
		SettingsImageButton.HoveredCursor = ImGuiMouseCursor_Hand;
		SettingsImageButton.UniqueID = "##SettingsImageButton";
		SettingsImageButton.Texture = FGui::GetTexture("SettingsIcon512");
		SettingsImageButton.Size = ImVec2((float)SettingsImageButton.Texture.Width, (float)SettingsImageButton.Texture.Height) / 24;
		SettingsImageButton.ImageColor = ImVec4(0.75f, 0.75f, 0.75f, 1.f);
		SettingsImageButton.HoveredImageColor = ImVec4(1.f, 1.f, 1.f, 1.f);
		SettingsImageButton.ButtonColor = ImVec4();
		SettingsImageButton.ActiveButtonColor = SettingsImageButton.ButtonColor;
		SettingsImageButton.HoveredButtonColor = SettingsImageButton.ButtonColor;
		SettingsImageButton.OnClickEvent = [this]() { MainWindowSpec.CurrentPage = FMainWindowSpec::EPageType::Settings; };

		FButton SettingsButton;
		SettingsButton.HoveredCursor = ImGuiMouseCursor_Hand;
		SettingsButton.Text = "Settings";
		SettingsButton.Font = GithubLinkButton.Font;
		SettingsButton.TextColor = ImVec4(0.75f, 0.75f, 0.75f, 1.f);
		SettingsButton.HoveredTextColor = ImVec4(1.f, 1.f, 1.f, 1.f);
		SettingsButton.Color = ImVec4();
		SettingsButton.ActiveColor = SettingsButton.Color;
		SettingsButton.HoveredColor = SettingsButton.Color;
		SettingsButton.OnClickEvent = SettingsImageButton.OnClickEvent;
		MainPage.AddImageButton("Settings", std::move(SettingsImageButton));
		MainPage.AddButton("Settings", std::move(SettingsButton));

		FGui::AddPage("Main", std::move(MainPage), [this]() { DrawMainPage(); });
		MainWindowSpec.PageTypeToIDMap[FMainWindowSpec::EPageType::Main] = "Main";
	}

	void FLauncher::CreateSettingsPage()
	{
		FPage SettingsPage;

		SettingsPage.SetTopPanel(CreateTopPanel([]() { FGui::Shutdown(); }));

		FButton BackButton;
		BackButton.HoveredCursor = ImGuiMouseCursor_Hand;
		BackButton.Text = "##Back";
		BackButton.Font = FGui::GetFont(22);
		BackButton.TextColor = ImVec4(0.75f, 0.75f, 0.75f, 1.f);
		BackButton.HoveredTextColor = BackButton.TextColor;
		BackButton.Color = ImVec4(0.1f, 0.1f, 0.1f, 1.f);
		BackButton.ActiveColor = BackButton.Color;
		BackButton.HoveredColor = ImVec4(0.15f, 0.15f, 0.15f, 1.f);
		BackButton.OnClickEvent = [this]() { MainWindowSpec.CurrentPage = FMainWindowSpec::EPageType::Main; };
		SettingsPage.AddButton("Back", std::move(BackButton));

		FImage BackImage;
		BackImage.HoveredCursor = ImGuiMouseCursor_Hand;
		BackImage.Texture = FGui::GetTexture("BackIcon512");
		BackImage.Size = ImVec2((float)BackImage.Texture.Width, (float)BackImage.Texture.Height) / 60.f;
		BackImage.Color = ImVec4(0.75f, 0.75f, 0.75f, 1.f);
		BackImage.HoveredColor = ImVec4(1.f, 1.f, 1.f, 1.f);
		SettingsPage.AddImage("Back", std::move(BackImage));

		FLine BackLine;
		BackLine.Color = ImVec4(0.75f, 0.75f, 0.75f, 1.f);
		BackLine.Thickness = 0.45f;
		SettingsPage.AddLine("Back", std::move(BackLine));

		FText BackText;
		BackText.HoveredCursor = ImGuiMouseCursor_Hand;
		BackText.Value = "Settings";
		BackText.Font = BackButton.Font;
		BackText.Color = ImVec4(0.75f, 0.75f, 0.75f, 1.f);
		BackText.HoveredColor = ImVec4(1.f, 1.f, 1.f, 1.f);
		SettingsPage.AddText("Back", std::move(BackText));

		FText ProjectsPathText;
		ProjectsPathText.Value = "Projects Path: ";
		ProjectsPathText.Font = FGui::GetFont(18);
		ProjectsPathText.Color = ImVec4(0.75f, 0.75f, 0.75f, 1.f);
		ProjectsPathText.HoveredColor = ProjectsPathText.Color;
		SettingsPage.AddText("ProjectsPath", std::move(ProjectsPathText));

		FInputTextBox ProjectsPathInputTextBox;
		ProjectsPathInputTextBox.Label = "##ProjectsPath";
		ProjectsPathInputTextBox.Hint = "Projects Path";
		ProjectsPathInputTextBox.Font = FGui::GetFont(18);
		ProjectsPathInputTextBox.Buffer = Settings.ProjectsFolderPath;
		ProjectsPathInputTextBox.BufferSize = Settings.ProjectsFolderPathSize;
		ProjectsPathInputTextBox.Color = ImVec4(0.2f, 0.2f, 0.2f, 1.f);
		SettingsPage.AddInputTextBox("ProjectsPath", std::move(ProjectsPathInputTextBox));

		FGui::AddPage("Settings", std::move(SettingsPage), [this]() { DrawSettingsPage(); });
		MainWindowSpec.PageTypeToIDMap[FMainWindowSpec::EPageType::Settings] = "Settings";
	}

	void FLauncher::CreateAddProjectPage()
	{

	}

	void FLauncher::CreateNewProjectPage()
	{
		FPage NewProjectPage;

		NewProjectPage.SetTopPanel(CreateTopPanel([this]() { bNewProjectWindowOpened = false; }));

		FInputTextBox ProjectNameInputTextBox;
		ProjectNameInputTextBox.Label = "##ProjectName";
		ProjectNameInputTextBox.Hint = "Project name";
		ProjectNameInputTextBox.Font = FGui::GetFont(18);
		ProjectNameInputTextBox.Buffer = std::make_shared<char[]>(1024);
		ProjectNameInputTextBox.BufferSize = 1024;
		ProjectNameInputTextBox.Color = ImVec4(0.2f, 0.2f, 0.2f, 1.f);
		NewProjectPage.AddInputTextBox("ProjectName", std::move(ProjectNameInputTextBox));

		FButton CreateButton;
		CreateButton.HoveredCursor = ImGuiMouseCursor_Hand;
		CreateButton.Text = "Create";
		CreateButton.Font = ProjectNameInputTextBox.Font;
		CreateButton.TextColor = ImVec4(0.f, 0.f, 0.f, 1.f);
		CreateButton.HoveredTextColor = CreateButton.TextColor;
		CreateButton.Color = (ImVec4)ImColor(239, 216, 0);
		CreateButton.ActiveColor = (ImVec4)ImColor(216, 198, 0);
		CreateButton.HoveredColor = (ImVec4)ImColor(245, 228, 0);
		CreateButton.OnClickEvent = [this]() { NewProject(); bNewProjectWindowOpened = false; };
		NewProjectPage.AddButton("Create", std::move(CreateButton));

		FGui::AddPage("NewProject", std::move(NewProjectPage), [this]() { DrawNewProjectPage(); });
		NewProjectWindowSpec.PageTypeToIDMap[FNewProjectWindowSpec::EPageType::Main] = "NewProject";
	}

	void FLauncher::CreateOpenProjectPage()
	{

	}

	void FLauncher::DrawTopPanel(const FPage& Page)
	{
		const FTopPanel& TopPanel = Page.GetTopPanel();
		const ImVec2& TopPanelOffset = Page.TopPanelOffset;
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

	void FLauncher::DrawMainPage()
	{
		FPage& CurrentPage = FGui::GetPage(MainWindowSpec.PageTypeToIDMap[MainWindowSpec.CurrentPage]);
		const ImVec2& TopOffset = CurrentPage.TopOffset;

		FGui::SetFramePaddingMultiplier(4);
		FGui::SetCursorPosition(TopOffset);

		const ImVec2& FramePadding = FGui::GetFramePadding();

		FGui::MoveCursor(-FramePadding);

		const FImageButton& LogoImageButton = CurrentPage.GetImageButton("Logo");
		const ImVec2& LogoImageButtonSize = FGui::CalculateSize(LogoImageButton);
		const FButton& GithubLinkButton = CurrentPage.GetButton("GithubLink");
		const ImVec2& GithubLinkButtonSize = FGui::CalculateSize(GithubLinkButton);
		const float CenteredButtonsLocalPositionY = FGui::GetCursorPosition().y
			+ LogoImageButtonSize.y * 0.5f
			- GithubLinkButtonSize.y * 0.5f;

		FGui::Draw(LogoImageButton);

		FGui::KeepLine();
		FGui::MoveCursorX(-FramePadding.x);
		FGui::SetCursorPositionY(CenteredButtonsLocalPositionY);
		FGui::Draw(GithubLinkButton);
		FGui::UpdateState(2);

		FGui::KeepLine();
		FGui::MoveCursorX(TopOffset.x - FramePadding.x / 2);
		FGui::SetCursorPositionY(CenteredButtonsLocalPositionY);

		const ImVec2& WindowPosition = FGui::GetWindowPosition();
		const ImVec2& WindowSize = FGui::GetWindowSize();
		const ImVec2& WindowCornerGlobalPosition = WindowPosition + WindowSize;
		const ImVec2& ProjectsButtonLocalPosition = FGui::GetCursorPosition();
		const ImVec2& ProjectsButtonGlobalPosition = WindowPosition + ProjectsButtonLocalPosition;

		const FButton& ProjectsButton = CurrentPage.GetButton("Projects");
		const ImVec2& ProjectsButtonSize = FGui::CalculateSize(ProjectsButton);

		FGui::Draw(CurrentPage.GetLine("HorizontalMainSeparator"),
			ImVec2(ProjectsButtonGlobalPosition.x, ProjectsButtonGlobalPosition.y + ProjectsButtonSize.y),
			ImVec2(WindowCornerGlobalPosition.x - 5.f, ProjectsButtonGlobalPosition.y + ProjectsButtonSize.y));
		FGui::Draw(CurrentPage.GetLine("VerticalMainSeparator"),
			ProjectsButtonGlobalPosition,
			ImVec2(ProjectsButtonGlobalPosition.x, WindowCornerGlobalPosition.y - 5.f));
		FGui::Draw(ProjectsButton);
		FGui::UpdateState();

		FGui::KeepLine();
		FGui::SetCursorPositionY(CenteredButtonsLocalPositionY);

		const FButton& ChangelogButton = CurrentPage.GetButton("Changelog");

		FGui::Draw(ChangelogButton);
		FGui::UpdateState();
		FGui::SetFramePaddingMultiplier(1);

		FGui::SetCursorPosition(ProjectsButtonLocalPosition);
		FGui::MoveCursorY(ProjectsButtonSize.y);

		switch (MainWindowSpec.MainPageSpec.CurrentTab)
		{
			case FMainWindowSpec::FMainPageSpec::ETabType::Projects:
			{
				UpdateProjectList();

				const FWidgetGroup& ProjectButtonGroup = CurrentPage.GetWidgetGroup("ProjectList");

				const ImVec2& BottomOffset = CurrentPage.BottomOffset;
				const ImVec2& CornerProjectListPosition = FGui::GetCursorPosition();

				const ImVec2& FullTextSize = FGui::CalculateSize(CurrentPage.GetText("_ProjectButtonNameTextDummy"))
					+ FGui::CalculateSize(CurrentPage.GetText("_ProjectButtonPathTextDummy"));
				const ImVec2& ButtonSize = ImVec2(FGui::GetWindowSize().x - CornerProjectListPosition.x,
					FullTextSize.y + FramePadding.y * 2.f);

				const FButton& NewProjectButton = CurrentPage.GetButton("NewProject");
				const ImVec2& NewProjectButtonSize = FGui::CalculateSize(NewProjectButton);

				FGui::MoveCursorX(ButtonSize.x - NewProjectButtonSize.x - BottomOffset.x);
				FGui::MoveCursorY(ButtonSize.y / 2.f - NewProjectButtonSize.y / 2.f);

				const ImVec2& NewProjectButtonLocalPosition = FGui::GetCursorPosition();

				FGui::Draw(NewProjectButton, NewProjectButtonSize);

				const FButton& AddProjectButton = CurrentPage.GetButton("AddProject");
				const ImVec2& AddProjectButtonSize = FGui::CalculateSize(AddProjectButton);
				const ImVec2& AddProjectButtonExtendedSize = ImVec2(AddProjectButtonSize.x * 2.f, AddProjectButtonSize.y);

				FGui::SetCursorPosition(NewProjectButtonLocalPosition);
				FGui::MoveCursorX(-AddProjectButtonExtendedSize.x - 4.f);
				FGui::SetCursorPositionY(CornerProjectListPosition.y);
				FGui::MoveCursorY(ButtonSize.y / 2.f - AddProjectButtonExtendedSize.y / 2.f);
				FGui::Draw(AddProjectButton, AddProjectButtonExtendedSize);

				if (!ProjectButtonGroup.GetSize()) break;

				FGui::SetCursorPosition(CornerProjectListPosition);
				FGui::MoveCursorY(ButtonSize.y);

				ImGui::BeginGroup();
				{
					for (size_t i = 0; i < ProjectList.size(); ++i)
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
				ImGui::EndGroup();

				break;
			}
			case FMainWindowSpec::FMainPageSpec::ETabType::Changelog:
			{
				break;
			}
		}

		const ImVec2& BottomOffset = CurrentPage.BottomOffset;
		const FImageButton& SettingsImageButton = CurrentPage.GetImageButton("Settings");
		const ImVec2& SettingsImageButtonSize = FGui::CalculateSize(SettingsImageButton);

		const FButton& SettingsButton = CurrentPage.GetButton("Settings");
		const ImVec2& SettingsButtonSize = FGui::CalculateSize(SettingsButton);

		FGui::SetCursorPositionX(ProjectsButtonLocalPosition.x / 2.f
			- SettingsImageButtonSize.x / 2.f
			- SettingsButtonSize.x / 2.f);
		FGui::SetCursorPositionY(WindowSize.y - BottomOffset.y - SettingsImageButtonSize.y);
		FGui::Draw(SettingsImageButton);

		FGui::KeepLine();
		FGui::MoveCursorY(-0.5f);
		FGui::Draw(SettingsButton);
		FGui::UpdateState(2);

		const FButton& LaunchButton = CurrentPage.GetButton("Launch");
		const ImVec2& LaunchButtonSize = FGui::CalculateSize(LaunchButton);

		FGui::SetCursorPosition(WindowSize - LaunchButtonSize - BottomOffset);
		FGui::Draw(LaunchButton);
		FGui::UpdateState();

		FGui::SetCursorPosition(ProjectsButtonLocalPosition);
		FGui::MoveCursorY(ProjectsButtonSize.y);
		FGui::MoveCursor(-1.f, -1.f);

		DrawTopPanel(CurrentPage);
	}

	void FLauncher::DrawSettingsPage()
	{
		const FPage& CurrentPage = FGui::GetPage(MainWindowSpec.PageTypeToIDMap[MainWindowSpec.CurrentPage]);
		const ImVec2& TopOffset = CurrentPage.TopOffset;

		const ImVec2& WindowSize = FGui::GetWindowSize();
		const FButton& BackButton = CurrentPage.GetButton("Back");
		const ImVec2& BackButtonSize = ImVec2(WindowSize.x, 40.f);

		FGui::SetFramePaddingMultiplier(4);
		FGui::SetCursorPosition(0.f, TopOffset.y);
		FGui::Draw(BackButton, BackButtonSize);

		const FImage& BackImage = CurrentPage.GetImage("Back");
		const ImVec2& BackImageSize = FGui::CalculateSize(BackImage);

		FGui::SetCursorPosition(TopOffset);
		FGui::MoveCursorY(BackButtonSize.y / 2.f - BackImageSize.y / 2.f);
		FGui::Draw(BackImage);

		const ImVec2& FramePadding = FGui::GetFramePadding();

		FGui::KeepLine();
		FGui::SetCursorPosition(TopOffset);
		FGui::MoveCursorX(BackImageSize.x + FramePadding.x);
		FGui::MoveCursorY(BackButtonSize.y / 2.f - BackButton.Font->FontSize / 2);

		const ImVec2& CursorGlobalPosition = FGui::GetWindowPosition() + FGui::GetCursorPosition();

		FGui::Draw(CurrentPage.GetLine("Back"),
			ImVec2(CursorGlobalPosition.x, CursorGlobalPosition.y),
			ImVec2(CursorGlobalPosition.x, CursorGlobalPosition.y + BackButton.Font->FontSize - 1.f));

		FGui::MoveCursorX(FramePadding.x);

		const FText& BackText = CurrentPage.GetText("Back");

		FGui::SetCursorPositionY(TopOffset.y + BackButtonSize.y / 2.f - FGui::CalculateSize(BackText).y / 2.f);
		FGui::Draw(BackText);
		FGui::UpdateState(3);
		FGui::SetFramePaddingMultiplier(1);

		FGui::SetCursorPosition(TopOffset);
		FGui::MoveCursorY(BackButtonSize.y);
		FGui::MoveCursorY(BackButtonSize.y / 2.f);

		const FText& ProjectsPathText = CurrentPage.GetText("ProjectsPath");
		const ImVec2& ProjectsPathTextLocalPosition = FGui::GetCursorPosition();

		FGui::Draw(ProjectsPathText);

		const FInputTextBox& ProjectsPathInputTextBox = CurrentPage.GetInputTextBox("ProjectsPath");
		const ImVec2& ProjectsPathTextSize = FGui::CalculateSize(ProjectsPathText);

		FGui::SetCursorPosition(ProjectsPathTextLocalPosition);
		FGui::MoveCursorX(ProjectsPathTextSize.x + FGui::GetFramePadding().x);
		FGui::MoveCursorY(ProjectsPathTextSize.y / 2 - FGui::CalculateSize(ProjectsPathInputTextBox).y / 2);
		FGui::Draw(ProjectsPathInputTextBox);

		DrawTopPanel(CurrentPage);
	}

	void FLauncher::DrawAddProjectPage()
	{
		//const FPage& CurrentPage = FGui::GetPage(MainWindowSpec.PageTypeToIDMap[MainWindowSpec.CurrentPage]);
		//const ImVec2& TopOffset = CurrentPage.TopOffset;
	}

	void FLauncher::DrawNewProjectPage()
	{
		const FPage& CurrentPage = FGui::GetPage(NewProjectWindowSpec.PageTypeToIDMap[NewProjectWindowSpec.CurrentPage]);
		const ImVec2& TopOffset = CurrentPage.TopOffset;

		const FInputTextBox& ProjectNameInputTextBox = CurrentPage.GetInputTextBox("ProjectName");

		FGui::SetCursorPosition(TopOffset);
		FGui::Draw(ProjectNameInputTextBox);
		//FGui::SetCursorPositionX(TopOffset.x);
		//FGui::Draw(CurrentPage.GetInputTextBox("ProjectPath"));

		const FButton& CreateButton = CurrentPage.GetButton("Create");
		const ImVec2& CreateButtonSize = FGui::CalculateSize(CreateButton);
		const ImVec2& BottomOffset = CurrentPage.BottomOffset;

		if (CreateButton.State.IsClicked())
		{
			NewProjectNameHolder = ProjectNameInputTextBox.Buffer.get();
		}

		FGui::SetCursorPosition(FGui::GetWindowSize() - CreateButtonSize - BottomOffset);
		FGui::Draw(CreateButton);
		FGui::UpdateState();

		DrawTopPanel(CurrentPage);
	}

	void FLauncher::DrawOpenProjectPage()
	{
		//const FPage& CurrentPage = FGui::GetPage(PageTypeToIDMap[this->CurrentPage]);
		//const ImVec2& TopOffset = CurrentPage.TopOffset;
	}
}
