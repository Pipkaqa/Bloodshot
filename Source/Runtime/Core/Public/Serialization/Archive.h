#pragma once

#include "Logging/LoggingMacros.h"
#include "Serialization/Encoder.h"
#include "String/Format.h"
#include "String/LowLevelString.h"

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

		FORCEINLINE ~FArchive()
		{
			if (bNeedFlush) Flush();
			Stream.close();
		}

		template<typename T>
		void Serialize(FStringView InName, const T& Object)
		{
			NodeTree.insert_or_assign(InName, TEncoder<T>().Encode(Object));
			bNeedFlush = true;
		}

		template<typename T>
		T Deserialize(FStringView InName)
		{
			BS_LOG_IF(NodeTree.find(InName) == NodeTree.end(), Fatal, "Trying to deserialize not existing value by key: {}", InName);
			const FEncodedNode& Node = NodeTree.at(InName);
			T Result = TEncoder<T>().Decode(Node);
			return Result;
		}

		void Flush();

	private:
		std::filesystem::path FilePath;
		std::fstream Stream;
		FString Buffer;

		TList<size_t> WriteHistoryStack;
		size_t IndentLevel = 0;
		bool bNeedFlush = false;

		TUnorderedMap<FString, FEncodedNode> NodeTree;

		void WriteNodeDataRecursive(const FEncodedNode& Node);

		FORCEINLINE void Write(FStringView String)
		{
			Buffer += String;
		}

		template<typename... ArgTypes>
		void WriteFmt(const char* InFmt, ArgTypes&&... Args)
		{
			Private::String::FLowLevelString Result = Private::String::LLFormat(InFmt, std::forward<ArgTypes>(Args)...);
			Write(Result.Data);
			Result.Release();
		}

		FORCEINLINE void NewLine()
		{
			Buffer += "\n";
			for (size_t i = 0; i < IndentLevel; ++i)
			{
				Buffer += "  ";
			}
		}

		FORCEINLINE void PushIndent()
		{
			++IndentLevel;
		}

		FORCEINLINE void PopIndent()
		{
			BS_ASSERT(IndentLevel, "FArchive: PopIndent() called when IndentLevel == 0");
			--IndentLevel;
		}
	};
}
