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

	template<typename ElementType, size_t Size>
	struct TEncoder<TArray<ElementType, Size>> final
	{
		using FArray = TArray<ElementType, Size>;
		using FElementEncoder = TEncoder<std::decay_t<ElementType>>;

		FEncodedNode Encode(const FArray& Array)
		{
			FEncodedNode Result;

			for (const ElementType& Element : Array)
			{
				Result.Children.emplace_back(FElementEncoder().Encode(Element));
			}

			return Result;
		}

		FArray Decode(const FEncodedNode& Node)
		{
			FArray Result;

			for (size_t i = 0; i < Node.Children.size(); ++i)
			{
				Result.at(i) = std::move(FElementEncoder().Decode(Node.Children.at(i)));
			}

			return Result;
		}
	};

	template<typename ElementType>
	struct TEncoder<TList<ElementType>> final
	{
		using FList = TList<ElementType>;
		using FElementEncoder = TEncoder<std::decay_t<ElementType>>;

		FEncodedNode Encode(const FList& List)
		{
			FEncodedNode Result;

			for (const ElementType& Element : List)
			{
				Result.Children.emplace_back(FElementEncoder().Encode(Element));
			}

			return Result;
		}

		FList Decode(const FEncodedNode& Node)
		{
			FList Result;

			for (const FEncodedNode& Node : Node.Children)
			{
				Result.emplace_back(std::move(FElementEncoder().Decode(Node)));
			}

			return Result;
		}
	};

	template<typename KeyType, typename ValueType>
	struct TEncoder<TMap<KeyType, ValueType>> final
	{
		using FMap = TMap<KeyType, ValueType>;
		using FKeyEncoder = TEncoder<std::decay_t<KeyType>>;
		using FValueEncoder = TEncoder<std::decay_t<ValueType>>;

		FEncodedNode Encode(const FMap& Map)
		{
			FEncodedNode Result;
			TVector<FEncodedNode>& Children = Result.Children;

			for (const TPair<const KeyType, ValueType>& ElementPair : Map)
			{
				FEncodedNode& PairNode = Children.emplace_back();
				PairNode.Children.emplace_back(FKeyEncoder().Encode(ElementPair.first));
				PairNode.Children.emplace_back(FValueEncoder().Encode(ElementPair.second));
			}

			return Result;
		}

		FMap Decode(const FEncodedNode& Node)
		{
			FMap Result;
			const TVector<FEncodedNode>& Children = Node.Children;

			for (const FEncodedNode& Node : Children)
			{
				KeyType Key = FKeyEncoder().Decode(Node.Children.at(0));
				ValueType Value = FValueEncoder().Decode(Node.Children.at(1));
				Result.emplace(std::move(Key), std::move(Value));
			}

			return Result;
		}
	};

	template<typename KeyType, typename ValueType>
	struct TEncoder<TUnorderedMap<KeyType, ValueType>> final
	{
		using FUnorderedMap = TUnorderedMap<KeyType, ValueType>;
		using FKeyEncoder = TEncoder<std::decay_t<KeyType>>;
		using FValueEncoder = TEncoder<std::decay_t<ValueType>>;

		FEncodedNode Encode(const FUnorderedMap& UnorderedMap)
		{
			FEncodedNode Result;
			TVector<FEncodedNode>& Children = Result.Children;

			for (const TPair<const KeyType, ValueType>& ElementPair : UnorderedMap)
			{
				FEncodedNode& PairNode = Children.emplace_back();
				PairNode.Children.emplace_back(FKeyEncoder().Encode(ElementPair.first));
				PairNode.Children.emplace_back(FValueEncoder().Encode(ElementPair.second));
			}

			return Result;
		}

		FUnorderedMap Decode(const FEncodedNode& Node)
		{
			FUnorderedMap Result;
			const TVector<FEncodedNode>& Children = Node.Children;

			for (const FEncodedNode& Node : Children)
			{
				KeyType Key = FKeyEncoder().Decode(Node.Children.at(0));
				ValueType Value = FValueEncoder().Decode(Node.Children.at(1));
				Result.emplace(std::move(Key), std::move(Value));
			}

			return Result;
		}
	};

	template<typename FirstElementType, typename SecondElementType>
	struct TEncoder<TPair<FirstElementType, SecondElementType>> final
	{
		using FPair = TPair<FirstElementType, SecondElementType>;
		using FFirstElementEncoder = TEncoder<std::decay_t<FirstElementType>>;
		using FSecondElementEncoder = TEncoder<std::decay_t<SecondElementType>>;

		FEncodedNode Encode(const FPair& Pair)
		{
			FEncodedNode Result;
			TVector<FEncodedNode>& Children = Result.Children;

			Children.emplace_back(FFirstElementEncoder().Encode(Pair.first));
			Children.emplace_back(FSecondElementEncoder().Encode(Pair.second));

			return Result;
		}

		FPair Decode(const FEncodedNode& Node)
		{
			const TVector<FEncodedNode>& Children = Node.Children;

			FirstElementType First = FFirstElementEncoder().Decode(Children.at(0));
			SecondElementType Second = FSecondElementEncoder().Decode(Children.at(1));
			FPair Result = std::make_pair(std::move(First), std::move(Second));

			return Result;
		}
	};

	template<typename ElementType>
	struct TEncoder<TSet<ElementType>> final
	{
		using FSet = TSet<ElementType>;
		using FElementEncoder = TEncoder<std::decay_t<ElementType>>;

		FEncodedNode Encode(const FSet& Set)
		{
			FEncodedNode Result;

			for (const ElementType& Element : Set)
			{
				Result.Children.emplace_back(FElementEncoder().Encode(Element));
			}

			return Result;
		}

		FSet Decode(const FEncodedNode& Node)
		{
			FSet Result;

			for (const FEncodedNode& Node : Node.Children)
			{
				Result.emplace(std::move(FElementEncoder().Decode(Node)));
			}

			return Result;
		}
	};

	template<typename ElementType>
	struct TEncoder<TUnorderedSet<ElementType>> final
	{
		using FUnorderedSet = TUnorderedSet<ElementType>;
		using FElementEncoder = TEncoder<std::decay_t<ElementType>>;

		FEncodedNode Encode(const FUnorderedSet& UnorderedSet)
		{
			FEncodedNode Result;

			for (const ElementType& Element : UnorderedSet)
			{
				Result.Children.emplace_back(FElementEncoder().Encode(Element));
			}

			return Result;
		}

		FUnorderedSet Decode(const FEncodedNode& Node)
		{
			FUnorderedSet Result;

			for (const FEncodedNode& Node : Node.Children)
			{
				Result.emplace(std::move(FElementEncoder().Decode(Node)));
			}

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

	template<typename ElementType>
	struct TEncoder<TVector<ElementType>> final
	{
		using FVector = TVector<ElementType>;
		using FElementEncoder = TEncoder<std::decay_t<ElementType>>;

		FEncodedNode Encode(const FVector& Vector)
		{
			FEncodedNode Result;

			for (const ElementType& Element : Vector)
			{
				Result.Children.emplace_back(FElementEncoder().Encode(Element));
			}

			return Result;
		}

		FVector Decode(const FEncodedNode& Node)
		{
			FVector Result;

			for (const FEncodedNode& Node : Node.Children)
			{
				Result.emplace(std::move(FElementEncoder().Decode(Node)));
			}

			return Result;
		}
	};
}
