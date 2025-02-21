#pragma once

#include "Containers/Array.h"
#include "Containers/List.h"
#include "Containers/Map.h"
#include "Containers/Pair.h"
#include "Containers/Set.h"
#include "Containers/StaticArray.h"
#include "Containers/String.h"
#include "Containers/StringView.h"
#include "Containers/Tuple.h"
#include "Containers/UnorderedMap.h"
#include "Containers/UnorderedSet.h"
#include "Templates/TypeTraits.h"

#include <filesystem>

namespace Bloodshot
{
	struct FEncodedNode final
	{
		FString Value;
		TArray<FEncodedNode> Children;
	};

	template<typename T>
	struct TEncoder final
	{
		static_assert(AssertFalse<T>::value, "Provide explicit specialization of TEncoder for T");

		FEncodedNode Encode(const T& Object);
		T Decode(const FEncodedNode& Node);
	};

	template<IsCharRange T>
	struct TEncoder<T> final
	{
		FEncodedNode Encode(const T& CharRange)
		{
			FEncodedNode Result;
			Result.Value = "\"" + CharRange + "\"";
			return Result;
		}

		T Decode(const FEncodedNode& Node)
		{
			const FString& Value = Node.Value;
			FStringView View = FStringView(Value.GetData() + 1, Value.GetSize() - 2);
			return View;
		}
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
	struct TEncoder<TStaticArray<ElementType, Size>> final
	{
		using FStaticArray = TStaticArray<ElementType, Size>;
		using FElementEncoder = TEncoder<std::decay_t<ElementType>>;

		FEncodedNode Encode(const FStaticArray& StaticArray)
		{
			FEncodedNode Result;

			for (const ElementType& Element : StaticArray)
			{
				Result.Children.EmplaceBack(FElementEncoder().Encode(Element));
			}

			return Result;
		}

		FStaticArray Decode(const FEncodedNode& Node)
		{
			FStaticArray Result;

			for (size_t i = 0; i < Node.Children.GetSize(); ++i)
			{
				Result.at(i) = std::move(FElementEncoder().Decode(Node.Children[i]));
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
				Result.Children.EmplaceBack(FElementEncoder().Encode(Element));
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
			TArray<FEncodedNode>& Children = Result.Children;

			for (const TPair<const KeyType, ValueType>& ElementPair : Map)
			{
				FEncodedNode& Pair = Children.EmplaceBackGetRef();
				TArray<FEncodedNode>& Underlying = Pair.Children;

				Underlying.EmplaceBack(FKeyEncoder().Encode(ElementPair.first));
				Underlying.EmplaceBack(FValueEncoder().Encode(ElementPair.second));
			}

			return Result;
		}

		FMap Decode(const FEncodedNode& Node)
		{
			FMap Result;
			const TArray<FEncodedNode>& Children = Node.Children;

			for (const FEncodedNode& Node : Children)
			{
				KeyType Key = FKeyEncoder().Decode(Node.Children[0]);
				ValueType Value = FValueEncoder().Decode(Node.Children[1]);
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
			TArray<FEncodedNode>& Children = Result.Children;

			for (const TPair<const KeyType, ValueType>& ElementPair : UnorderedMap)
			{
				FEncodedNode& Pair = Children.EmplaceBackGetRef();
				TArray<FEncodedNode>& Underlying = Pair.Children;

				Underlying.EmplaceBack(FKeyEncoder().Encode(ElementPair.first));
				Underlying.EmplaceBack(FValueEncoder().Encode(ElementPair.second));
			}

			return Result;
		}

		FUnorderedMap Decode(const FEncodedNode& Node)
		{
			FUnorderedMap Result;
			const TArray<FEncodedNode>& Children = Node.Children;

			for (const FEncodedNode& Node : Children)
			{
				KeyType Key = FKeyEncoder().Decode(Node.Children[0]);
				ValueType Value = FValueEncoder().Decode(Node.Children[1]);
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
			TArray<FEncodedNode>& Children = Result.Children;

			Children.EmplaceBack(FFirstElementEncoder().Encode(Pair.first));
			Children.EmplaceBack(FSecondElementEncoder().Encode(Pair.second));

			return Result;
		}

		FPair Decode(const FEncodedNode& Node)
		{
			const TArray<FEncodedNode>& Children = Node.Children;

			FirstElementType First = FFirstElementEncoder().Decode(Children[0]);
			SecondElementType Second = FSecondElementEncoder().Decode(Children[1]);
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
				Result.Children.EmplaceBack(FElementEncoder().Encode(Element));
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
				Result.Children.EmplaceBack(FElementEncoder().Encode(Element));
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
			TArray<FEncodedNode>& Children = Result.Children;

			auto Encode = [&Children](const auto& Arg)
			{
				Children.EmplaceBack(TEncoder<std::decay_t<decltype(Arg)>>().Encode(Arg));
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
				Arg = TEncoder<std::decay_t<decltype(Arg)>>().Decode(Node.Children[ArgIndex++]);
			};

			std::apply([&Decode](auto&... Args)
			{
				(Decode(Args), ...);
			}, Result);

			return Result;
		}
	};

	template<typename ElementType>
	struct TEncoder<TArray<ElementType>> final
	{
		using FArray = TArray<ElementType>;
		using FElementEncoder = TEncoder<std::decay_t<ElementType>>;

		FEncodedNode Encode(const FArray& Array)
		{
			FEncodedNode Result;

			for (const ElementType& Element : Array)
			{
				Result.Children.EmplaceBack(FElementEncoder().Encode(Element));
			}

			return Result;
		}

		FArray Decode(const FEncodedNode& Node)
		{
			FArray Result;

			for (const FEncodedNode& Node : Node.Children)
			{
				Result.emplace(std::move(FElementEncoder().Decode(Node)));
			}

			return Result;
		}
	};

	template<>
	struct TEncoder<std::filesystem::path> final
	{
		using FPath = std::filesystem::path;

		FEncodedNode Encode(const FPath& Path)
		{
			FEncodedNode Result;
			Result = TEncoder().Encode(Path.string());
			return Result;
		}

		FPath Decode(const FEncodedNode& Node)
		{
			FPath Result;
			Result = TEncoder<FString>().Decode(Node).GetData();
			return Result;
		}
	};
}
