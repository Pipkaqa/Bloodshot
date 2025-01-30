#pragma once

#include "Logging/LoggingMacros.h"
#include "Serialization/Encoder.h"

#include <filesystem>
#include <format>
#include <fstream>

namespace Bloodshot
{
	template<typename T>
	struct TEncoder;

	class FArchive final
	{
		template<typename T>
		friend struct TEncoder;

	public:
		FArchive(const std::filesystem::path& OutputPath);
		~FArchive();

		template<typename T>
		void Serialize(const T& Object, FStringView Name)
		{
			// BSTODO: Optimize when implement own containers

			NodeTree.insert_or_assign(FString(Name), TEncoder<T>().Encode(Object));
			bNeedFlush = true;
		}

		template<typename T>
		T Deserialize(FStringView Name)
		{
			// BSTODO: Optimize when implement own containers

			FString NameStr(Name);

			if (NodeTree.find(NameStr) == NodeTree.end())
			{
				BS_LOG(Fatal, "Trying to deserialize not existing value by key: {}", NameStr);
			}

			const FEncodedNode& Node = NodeTree.at(NameStr);
			T Result = TEncoder<T>().Decode(Node);
			return Result;
		}

		void Flush();

	private:
		std::filesystem::path FilePath;
		std::fstream Stream;
		FString Buffer;

		TList<size_t> WriteHistoryStack;
		size_t PushedScopes = 0;
		bool bNeedFlush = false;

		TUnorderedMap<FString, FEncodedNode> NodeTree;

		void WriteNodeDataRecursive(const FEncodedNode& Node);

		void Write(FStringView String);

		template<typename... ArgTypes>
		void Write(const std::format_string<ArgTypes...>& Format, ArgTypes&&... Args)
		{
			Write(std::format(Format, std::forward<ArgTypes>(Args)...));
		}

		void NewLine();
		void Undo();

		void PushScope();
		void PopScope();
	};

}
