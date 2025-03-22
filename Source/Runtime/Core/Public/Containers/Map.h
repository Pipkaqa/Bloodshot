#pragma once

//#include "Allocator.h"
//#include "AllocatorTraits.h"
//#include "Misc/AssertionMacros.h"
#include "Containers/Pair.h"
#include "Containers/Set.h"

#include <unordered_map>

namespace Bloodshot
{
	// BSTODO: Implement

	//template<typename InKeyType,
	//	typename InValueType,
	//	typename InPredicateType = std::equal_to<InKeyType>,
	//	typename InHashType = std::hash<InKeyType>,
	//	typename InAllocatorType = std::allocator<TPair<const InKeyType, InValueType>>>
	//using TMap = std::unordered_map<InKeyType, InValueType, InHashType, InPredicateType, InAllocatorType>;

	template<typename InKeyType, typename InValueType, bool bInAllowDuplicateKeys = false>
	struct TDefaultMapKeyFuncs : public TBaseKeyFuncs<InKeyType, TPair<InKeyType, InValueType>, bInAllowDuplicateKeys>
	{
		using KeyType = InKeyType;
		using ValueType = InValueType;

		using KeyInitType = TCallTraits<KeyType>::ConstPointerType;
		using ElementInitType = const TPair<KeyType, ValueType>&;

		NODISCARD FORCEINLINE static KeyInitType GetSetKey(ElementInitType Element)
		{
			return Element.First;
		}

		NODISCARD FORCEINLINE static uint64_t GetKeyHash(KeyInitType Key)
		{
			return GetTypeHash(Key);
		}

		template<typename ComparableKey>
		NODISCARD FORCEINLINE static uint64_t GetKeyHash(ComparableKey Key)
		{
			return GetTypeHash(Key);
		}

		NODISCARD FORCEINLINE static bool Matches(KeyInitType Left, KeyInitType Right)
		{
			return Left == Right;
		}

		template<typename ComparableKey>
		NODISCARD FORCEINLINE static bool Matches(KeyInitType Left, ComparableKey Right)
		{
			return Left == Right;
		}
	};

	template<typename InKeyType,
		typename InValueType,
		typename InKeyFuncs = TDefaultMapKeyFuncs<InKeyType, InValueType>,
		IsAllocator InAllocatorType = FDefaultAllocator,
		IsAllocator InHashAllocatorType = FDefaultAllocator>
	class TMap
	{
	public:
		using KeyType = InKeyType;
		using ValueType = InValueType;
		using ElementType = TPair<KeyType, ValueType>;
		using KeyFuncs = InKeyFuncs;
		using AllocatorType = InAllocatorType;
		using HashAllocatorType = InHashAllocatorType;

	private:
		using DataType = TSet<ElementType, KeyFuncs, AllocatorType, HashAllocatorType>;
		DataType Pairs;

		using KeyConstPointerType = TCallTraits<KeyType>::ConstPointerType;

	public:
		TMap() = default;
		TMap(TMap&&) = default;
		TMap(const TMap&) = default;
		TMap& operator=(TMap&&) = default;
		TMap& operator=(const TMap&) = default;

		FORCEINLINE ValueType& operator[](KeyConstPointerType Key)
		{
			return FindChecked(Key);
		}

		FORCEINLINE const ValueType& operator[](KeyConstPointerType Key) const
		{
			return FindChecked(Key);
		}

		NODISCARD FORCEINLINE size_t GetSize() const noexcept
		{
			return Pairs.GetSize();
		}

		NODISCARD FORCEINLINE bool IsEmpty() const noexcept
		{
			return Pairs.IsEmpty();
		}

		NODISCARD FORCEINLINE bool IsValidIndex(const size_t Index) const noexcept
		{
			return Pairs.IsValidIndex(Index);
		}

		NODISCARD FORCEINLINE ElementType& Get(const size_t Index)
		{
			return Pairs[Index];
		}

		NODISCARD FORCEINLINE const ElementType& Get(const size_t Index) const
		{
			return Pairs[Index];
		}

		FORCEINLINE void Reserve(const size_t NewCapacity)
		{
			Pairs.Reserve(NewCapacity);
		}

		FORCEINLINE ValueType& AddByHash(const uint64_t InKeyHash, const KeyType& InKey)
		{
			return EmplaceByHash(InKeyHash, InKey);
		}

		FORCEINLINE ValueType& AddByHash(const uint64_t InKeyHash, KeyType&& InKey)
		{
			return EmplaceByHash(InKeyHash, std::move(InKey));
		}

		FORCEINLINE ValueType& AddByHash(const uint64_t InKeyHash, const KeyType& InKey, const ValueType& InValue)
		{
			return EmplaceByHash(InKeyHash, InKey, InValue);
		}

		FORCEINLINE ValueType& AddByHash(const uint64_t InKeyHash, KeyType&& InKey, const ValueType& InValue)
		{
			return EmplaceByHash(InKeyHash, std::move(InKey), InValue);
		}

		FORCEINLINE ValueType& AddByHash(const uint64_t InKeyHash, const KeyType& InKey, ValueType&& InValue)
		{
			return EmplaceByHash(InKeyHash, InKey, std::move(InValue));
		}

		FORCEINLINE ValueType& AddByHash(const uint64_t InKeyHash, KeyType&& InKey, ValueType&& InValue)
		{
			return EmplaceByHash(InKeyHash, std::move(InKey), std::move(InValue));
		}

		FORCEINLINE ValueType& Add(const KeyType& InKey, const ValueType& InValue)
		{
			return Emplace(InKey, InValue);
		}

		FORCEINLINE ValueType& Add(const KeyType& InKey, ValueType&& InValue)
		{
			return Emplace(InKey, std::move(InValue));
		}

		FORCEINLINE ValueType& Add(KeyType&& InKey, const ValueType& InValue)
		{
			return Emplace(std::move(InKey), InValue);
		}

		FORCEINLINE ValueType& Add(KeyType&& InKey, ValueType&& InValue)
		{
			return Emplace(std::move(InKey), std::move(InValue));
		}

		FORCEINLINE ValueType& Add(const KeyType& InKey)
		{
			return Emplace(InKey);
		}

		FORCEINLINE ValueType& Add(KeyType&& InKey)
		{
			return Emplace(std::move(InKey));
		}

		template<typename InitKeyType = KeyType>
		FORCEINLINE ValueType& EmplaceByHash(const uint64_t InKeyHash, InitKeyType&& InKey)
		{
			const size_t PairIndex = Pairs.EmplaceByHash(InKeyHash, std::forward<InitKeyType>(InKey), ValueType());
			return Pairs[PairIndex].Second;
		}

		template<typename InitKeyType = KeyType, typename InitValueType = ValueType>
		FORCEINLINE ValueType& EmplaceByHash(const uint64_t InKeyHash, InitKeyType&& InKey, InitValueType&& InValue)
		{
			const size_t PairIndex = Pairs.EmplaceByHash(InKeyHash, std::forward<InitKeyType>(InKey), std::forward<InitValueType>(InValue));
			return Pairs[PairIndex].Second;
		}

		template<typename InitKeyType = KeyType>
		FORCEINLINE ValueType& Emplace(InitKeyType&& InKey)
		{
			const size_t PairIndex = Pairs.Emplace(std::forward<InitKeyType>(InKey), ValueType());
			return Pairs[PairIndex].Second;
		}

		template<typename InitKeyType = KeyType, typename InitValueType = ValueType>
		FORCEINLINE ValueType& Emplace(InitKeyType&& InKey, InitValueType&& InValue)
		{
			const size_t PairIndex = Pairs.Emplace(std::forward<InitKeyType>(InKey), std::forward<InitValueType>(InValue));
			return Pairs[PairIndex].Second;
		}

		template<typename ComparableKey>
		NODISCARD FORCEINLINE ValueType* FindByHash(const uint64_t KeyHash, const ComparableKey& Key)
		{
			if (ElementType* const Pair = Pairs.FindByHash(KeyHash, Key))
			{
				return &Pair->Second;
			}
			return nullptr;
		}

		template<typename ComparableKey>
		NODISCARD FORCEINLINE const ValueType* FindByHash(const uint64_t KeyHash, const ComparableKey& Key) const
		{
			return const_cast<TMap*>(this)->FindByHash(KeyHash, Key);
		}

		NODISCARD FORCEINLINE ValueType* Find(KeyConstPointerType Key)
		{
			if (ElementType* const Pair = Pairs.Find(Key))
			{
				return &Pair->Second;
			}
			return nullptr;
		}

		NODISCARD FORCEINLINE const ValueType* Find(KeyConstPointerType Key) const
		{
			return const_cast<TMap*>(this)->Find(Key);
		}

		NODISCARD FORCEINLINE ValueType& FindChecked(KeyConstPointerType Key)
		{
			ElementType* const Pair = Pairs.Find(Key);
			BS_CHECK(Pair);
			return Pair->Second;
		}

		NODISCARD FORCEINLINE const ValueType& FindChecked(KeyConstPointerType Key) const
		{
			return const_cast<TMap*>(this)->FindChecked(Key);
		}

		NODISCARD FORCEINLINE ValueType& FindOrAddByHash(const uint64_t KeyHash, const KeyType& Key)
		{
			return FindOrAddImpl(KeyHash, Key);
		}

		NODISCARD FORCEINLINE ValueType& FindOrAddByHash(const uint64_t KeyHash, KeyType&& Key)
		{
			return FindOrAddImpl(KeyHash, std::move(Key));
		}

		NODISCARD FORCEINLINE ValueType& FindOrAdd(const KeyType& Key)
		{
			return FindOrAddImpl(KeyFuncs::GetKeyHash(Key), Key);
		}

		NODISCARD FORCEINLINE ValueType& FindOrAdd(KeyType&& Key)
		{
			return FindOrAddImpl(KeyFuncs::GetKeyHash(Key), std::move(Key));
		}

		NODISCARD FORCEINLINE TArray<ElementType> ToArray() const
		{
			TArray<ElementType> Result;
			for (FConstIterator It = CreateConstIterator(); It; ++It)
			{
				Result.PushBack(*It);
			}
			return Result;
		}

		FORCEINLINE void Shrink()
		{
			//Pairs.Shrink();
		}

		FORCEINLINE void Clear()
		{
			Pairs.Clear();
		}

		FORCEINLINE void Dispose()
		{
			Pairs.Dispose();
		}

	private:
		template<bool bConst, bool bRangeFor = false>
		class TBaseIterator
		{
		public:
			using PairIteratorType = std::conditional_t<
				bConst,
				std::conditional_t<bRangeFor, typename DataType::FRangeForConstIterator, typename DataType::FConstIterator>,
				std::conditional_t<bRangeFor, typename DataType::FRangeForIterator, typename DataType::FIterator>>;

		protected:
			using MapType = std::conditional_t<bConst, const TMap, TMap>;
			using IteratorKeyType = std::conditional_t<bConst, const KeyType, KeyType>;
			using IteratorValueType = std::conditional_t<bConst, const ValueType, ValueType>;
			using PairType = std::conditional_t<bConst, const typename DataType::ElementType, typename DataType::ElementType>;

		public:
			FORCEINLINE TBaseIterator(const PairIteratorType& InPairIt)
				: PairIt(InPairIt)
			{
			}

			NODISCARD FORCEINLINE PairType& operator*() const
			{
				return *PairIt;
			}

			NODISCARD FORCEINLINE PairType* operator->() const
			{
				return &*PairIt;
			}

			FORCEINLINE TBaseIterator& operator++()
			{
				++PairIt;
				return *this;
			}

			NODISCARD FORCEINLINE bool operator==(const TBaseIterator& Other) const
			{
				return PairIt == Other.PairIt;
			}

			NODISCARD FORCEINLINE bool operator!=(const TBaseIterator& Other) const
			{
				return PairIt != Other.PairIt;
			}

			NODISCARD FORCEINLINE explicit operator bool() const
			{
				return (bool)PairIt;
			}

			NODISCARD FORCEINLINE bool operator !() const
			{
				return !(bool)*this;
			}

			NODISCARD FORCEINLINE size_t GetIndex() const noexcept
			{
				return PairIt->GetIndex();
			}

			NODISCARD FORCEINLINE IteratorKeyType& GetKey() const
			{
				return PairIt->First;
			}

			NODISCARD FORCEINLINE IteratorValueType& GetValue() const
			{
				return PairIt->Second;
			}

		protected:
			PairIteratorType PairIt;
		};

	public:
		class FIterator : public TBaseIterator<false>
		{
			using Super = TBaseIterator<false>;
			using MapType = Super::MapType;

		public:
			FORCEINLINE FIterator(MapType& InMap)
				: Super(InMap.Pairs.CreateIterator())
				, Map(InMap)
			{
			}

			FORCEINLINE void RemoveCurrent()
			{
				Super::PairIt.RemoveCurrent();
			}

		private:
			MapType& Map;
		};

		class FConstIterator : public TBaseIterator<true>
		{
			using Super = TBaseIterator<true>;
			using MapType = Super::MapType;

		public:
			FORCEINLINE FConstIterator(MapType& InMap)
				: Super(InMap.Pairs.CreateConstIterator())
			{
			}
		};

		using FRangeForIterator = TBaseIterator<false, true>;
		using FRangeForConstIterator = TBaseIterator<true, true>;

		NODISCARD FORCEINLINE FIterator CreateIterator()
		{
			return FIterator(*this);
		}

		NODISCARD FORCEINLINE FConstIterator CreateConstIterator() const
		{
			return FConstIterator(*this);
		}

		// For internal usage only!
		NODISCARD FORCEINLINE FRangeForIterator begin() { return FRangeForIterator(Pairs.begin()); }
		NODISCARD FORCEINLINE FRangeForConstIterator begin() const { return FRangeForConstIterator(Pairs.begin()); }
		NODISCARD FORCEINLINE FRangeForIterator end() { return FRangeForIterator(Pairs.end()); }
		NODISCARD FORCEINLINE FRangeForConstIterator end() const { return FRangeForConstIterator(Pairs.end()); }

	private:
		template<typename InitKeyType>
		NODISCARD ValueType& FindOrAddImpl(const uint64_t KeyHash, InitKeyType&& Key)
		{
			if (ElementType* const Pair = Pairs.FindByHash(KeyHash, Key))
			{
				return Pair->Second;
			}
			return AddByHash(KeyHash, std::forward<InitKeyType>(Key));
		}

		template<typename InitKeyType, typename InitValueType>
		NODISCARD ValueType& FindOrAddImpl(const uint64_t KeyHash, InitKeyType&& Key, InitValueType&& Value)
		{
			if (ElementType* const Pair = Pairs.FindByHash(KeyHash, Key))
			{
				return Pair->Second;
			}
			return AddByHash(KeyHash, std::forward<InitKeyType>(Key), std::forward<InitValueType>(Value));
		}
	};
}
