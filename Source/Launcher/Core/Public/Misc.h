#pragma once

#include "OpenGL/ImGuiHeader.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace Bloodshot::Launcher
{
	class FProject final
	{
	public:
		FProject() {}

		std::string Name = "Untitled";

		std::filesystem::path Directory;
		std::filesystem::path ContentDirectory;
		std::filesystem::path StartSceneDirectory;
	};

	inline std::string ToString(const std::filesystem::path& Path)
	{
		std::u8string UTF8String = Path.u8string();
		return std::string(reinterpret_cast<char const*>(UTF8String.data()), UTF8String.size());
	}

	inline std::vector<std::string> Split(std::string& String, const std::string& Separator)
	{
		std::vector<std::string> Words;
		size_t Position = 0;
		std::string Word;

		while ((Position = String.find(Separator)) != std::string::npos)
		{
			Word = String.substr(0, Position);
			Words.push_back(Word);
			String.erase(0, Position + Separator.length());
		}

		Words.push_back(String);

		return Words;
	}

	inline void DrawTestWindow(std::string& ProjectPathText, std::string& LastErrorText)
	{
		ImGui::SetNextWindowSize(ImVec2(700, 250));

		if (ImGui::Begin("Test", nullptr))
		{
			ImGui::Text(std::format("Last Error: {}", LastErrorText.data()).c_str());

			if (ImGui::InputText("Enter Project Path", ProjectPathText.data(), CHAR_MAX))
			{
			}

			if (ImGui::Button("Create New Project"))
			{
			}

			if (ImGui::Button("Open Project"))
			{
				if (!std::filesystem::exists(ProjectPathText.data()))
				{
					LastErrorText = "Project Not Exists";
				}
				else
				{
					FProject Project;

					std::ifstream InputFileStream(ProjectPathText.data());
					std::string Line;

					std::getline(InputFileStream, Line);
					std::vector<std::string> Words = Split(Line, " ");
					Project.Name = Words[1];

					std::getline(InputFileStream, Line);
					Words = Split(Line, " ");
					Project.Directory = Words[1];

					std::getline(InputFileStream, Line);
					Words = Split(Line, " ");
					Project.ContentDirectory = Words[1];

					std::getline(InputFileStream, Line);
					Words = Split(Line, " ");
					Project.StartSceneDirectory = Words[1];

					std::cout << std::format("Loaded Project: \n\tName: {}\n\tDirectory: {}\n\tContentDirectory: {}\n\tStartSceneDirectory:{}\n",
						Project.Name,
						Project.Directory.string(),
						Project.ContentDirectory.string(),
						Project.StartSceneDirectory.string());

					InputFileStream.close();
				}
			}

			if (ImGui::Button("Reset"))
			{
				ProjectPathText = "C:\\Users\\Pipka\\Desktop\\bsproject\\Example.bsproject";
				LastErrorText = "None";

				std::cout << "Reseted!\n";
			}
		}
		ImGui::End();
	}

	inline ImVec4 GetRainbowColor()
	{
		int FrameCount = ImGui::GetFrameCount();

		static float Red = 0.0f, Green = 0.01f, Blue = 0.0f;

		ImVec4 Color = ImVec4(Red, Green, Blue, 1.0f);

		if (FrameCount % 1 == 0)
		{
			if (Green == 0.01f && Blue == 0.0f)
			{
				Red += 0.01f;
			}

			if (Red > 0.99f && Blue == 0.0f)
			{
				Red = 1.0f;
				Green += 0.01f;
			}

			if (Green > 0.99f && Blue == 0.0f)
			{
				Green = 1.0f;
				Red -= 0.01f;
			}

			if (Red < 0.01f && Green == 1.0f)
			{
				Red = 0.0f;
				Blue += 0.01f;
			}

			if (Blue > 0.99f && Red == 0.0f)
			{
				Blue = 1.0f;
				Green -= 0.01f;
			}

			if (Green < 0.01f && Blue == 1.0f)
			{
				Green = 0.0f;
				Red += 0.01f;
			}

			if (Red > 0.99f && Green == 0.0f)
			{
				Red = 1.0f;
				Blue -= 0.01f;
			}

			if (Blue < 0.01f && Green == 0.0f)
			{
				Blue = 0.0f;
				Red -= 0.01f;

				if (Red < 0.01f) Green = 0.01f;
			}
		}

		return Color;
	}
}
