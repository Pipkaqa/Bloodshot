#pragma once

// BSTODO: Optimize includes

#include "Logging/LoggingMacros.h"
#include "Templates/Containers/Array.h"
#include "Templates/Containers/List.h"
#include "Templates/Containers/Map.h"
#include "Templates/Containers/Pair.h"
#include "Templates/Containers/Set.h"
#include "Templates/Containers/String.h"
#include "Templates/Containers/Tuple.h"
#include "Templates/Containers/Vector.h"

#include <filesystem>
#include <format>
#include <fstream>

namespace Bloodshot
{
	// BSTODO: Deserialization (maybe generate some functions), also need good API for overloading

	template <typename T>
	concept IsContainer = requires(T Container)
	{
		typename T::iterator;
		{ Container.begin() } -> std::same_as<typename T::iterator>;
		{ Container.end() } -> std::same_as<typename T::iterator>;
	};

	template<typename T>
	struct AssertFalse : std::false_type {};

	template<typename T>
	concept IsIntegral = std::is_integral_v<T>;

	template<typename T>
	concept IsFloatingPoint = std::is_floating_point_v<T>;

	template<typename T>
	struct TEncoder;

	struct FEncodedNode final
	{
		FString Value;
		TVector<FEncodedNode> Children;
	};

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

	template<typename T>
	struct TEncoder final
	{
		static_assert(AssertFalse<T>::value, "Provide explicit specialization of FEncoder for T");

		FEncodedNode Encode(const T& Object);
		T Decode(const FEncodedNode& Node);
	};

	template<>
	struct TEncoder<FString> final
	{
		FEncodedNode Encode(const FString& String)
		{
			FEncodedNode Result;
			Result.Value = "\"" + String + "\"";
			return Result;
		}

		FString Decode(const FEncodedNode& Node)
		{
			const FString& Value = Node.Value;
			return Value.substr(1, Value.length() - 2);
		}
	};

	template<>
	struct TEncoder<FStringView> final
	{
		FEncodedNode Encode(const FStringView& String)
		{
			FEncodedNode Result;
			Result.Value = "\"" + FString(String) + "\"";
			return Result;
		}

		FStringView Decode(const FEncodedNode& Node) = delete;
	};

	template<IsIntegral T>
	struct TEncoder<T> final
	{
		FEncodedNode Encode(const T& Integral)
		{
			FEncodedNode Result;
			Result.Value = std::format("{}", Integral);
			return Result;
		}

		T Decode(const FEncodedNode& Node)
		{
			return (T)std::stoll(Node.Value);
		}
	};

	template<IsFloatingPoint T>
	struct TEncoder<T> final
	{
		FEncodedNode Encode(const T& Float)
		{
			FEncodedNode Result;
			Result.Value = std::format("{}", Float);
			return Result;
		}

		T Decode(const FEncodedNode& Node)
		{
			return (T)std::stod(Node.Value);
		}
	};

	template<typename ElementType>
	struct TEncoder<TList<ElementType>> final
	{
		using FList = TList<ElementType>;
		using FElementTypeEncoder = TEncoder<std::decay_t<ElementType>>;

		FEncodedNode Encode(const FList& List)
		{
			FEncodedNode Result;

			for (const ElementType& Element : List)
			{
				Result.Children.emplace_back(FElementTypeEncoder().Encode(Element));
			}

			return Result;
		}

		FList Decode(const FEncodedNode& Node)
		{
			FList Result;

			for (const FEncodedNode& Child : Node.Children)
			{
				Result.emplace_back(std::move(FElementTypeEncoder().Decode(Child)));
			}

			return Result;
		}
	};

	template<typename FirstElementType, typename SecondElementType>
	struct TEncoder<TPair<FirstElementType, SecondElementType>> final
	{
		using FPair = TPair<FirstElementType, SecondElementType>;
		using FFirstElementTypeEncoder = TEncoder<std::decay_t<FirstElementType>>;
		using FSecondElementTypeEncoder = TEncoder<std::decay_t<SecondElementType>>;

		FEncodedNode Encode(const FPair& Pair)
		{
			FEncodedNode Result;
			TVector<FEncodedNode>& Children = Result.Children;

			Children.emplace_back(FFirstElementTypeEncoder().Encode(Pair.first));
			Children.emplace_back(FSecondElementTypeEncoder().Encode(Pair.second));

			return Result;
		}

		FPair Decode(const FEncodedNode& Node)
		{
			const TVector<FEncodedNode>& Children = Node.Children;

			FirstElementType First = FFirstElementTypeEncoder().Decode(Children.at(0));
			SecondElementType Second = FSecondElementTypeEncoder().Decode(Children.at(1));
			FPair Result = std::make_pair(std::move(First), std::move(Second));

			return Result;
		}
	};

	template<typename ElementType, typename... ElementTypes>
	struct TEncoder<TTuple<ElementType, ElementTypes...>> final
	{
		using FTuple = TTuple<ElementType, ElementTypes...>;

		FEncodedNode Encode(const FTuple& Tuple)
		{
			FEncodedNode Result;
			TVector<FEncodedNode>& Children = Result.Children;

			auto Encode = [&Children](const auto& Arg)
				{
					Children.emplace_back(TEncoder<std::decay_t<decltype(Arg)>>().Encode(Arg));
				};

			std::apply([&Encode](const auto&... Args)
				{
					(Encode(Args), ...);
				}, Tuple);

			return Result;
		}

		FTuple Decode(const FEncodedNode& Node)
		{
			FTuple Result;
			size_t ArgIndex = 0;

			auto Decode = [&Node, &ArgIndex](auto& Arg)
				{
					Arg = TEncoder<std::decay_t<decltype(Arg)>>().Decode(Node.Children.at(ArgIndex++));
				};

			std::apply([&Decode](auto&... Args)
				{
					(Decode(Args), ...);
				}, Result);

			return Result;
		}
	};
}
