#pragma once

#include "Templates/Template.h"
#include "Templates/TypeTraits.h"

#include <array>
#include <tuple>

namespace Bloodshot
{
	template<typename... ElementTypes>
	class TTuple
	{
		static constexpr TIndexRange<sizeof...(ElementTypes)> IndexSequence{};

	public:
		template<typename... ArgTypes>
			requires std::conjunction_v<std::is_convertible<ArgTypes&&, ElementTypes>...>
		constexpr TTuple(ArgTypes&&... Args)
		{
			Init(IndexSequence, std::forward<ArgTypes>(Args)...);
		}

	private:
		template<typename ElementType>
		FORCEINLINE static constexpr void CopySingle(ElementType& Left, const ElementType& Right)
		{
			if constexpr (!std::is_trivially_copyable_v<ElementType>)
			{
				Left = Right;
			}
			else
			{
				memcpy(&Left, &Right, sizeof(ElementType));
			}
		}

		template<size_t... Indices>
		FORCEINLINE constexpr void Copy(const TTuple& Other, TIndexSequence<Indices...>)
		{
			(CopySingle(*(ElementTypes*)(Storage + MembersInfo[Indices].Offset),
				*(ElementTypes*)(Other.Storage + MembersInfo[Indices].Offset)), ...);
		}

	public:
		FORCEINLINE constexpr TTuple(const TTuple& Other)
		{
			Copy(Other, IndexSequence);
		}

		FORCEINLINE constexpr TTuple& operator=(const TTuple& Other)
		{
			Copy(Other, IndexSequence);
			return *this;
		}

	private:
		template<typename ElementType>
		FORCEINLINE static constexpr void MoveSingle(ElementType& Left, ElementType&& Right)
		{
			Left = std::move(Right);
		}

		template<size_t... Indices>
		FORCEINLINE constexpr void Move(TTuple&& Other, TIndexSequence<Indices...>)
		{
			(MoveSingle(*(ElementTypes*)(Storage + MembersInfo[Indices].Offset),
				std::move(*(ElementTypes*)(Other.Storage + MembersInfo[Indices].Offset))), ...);
		}

	public:
		FORCEINLINE constexpr TTuple(TTuple&& Other) noexcept
		{
			Move(std::move(Other), IndexSequence);
		}

		FORCEINLINE constexpr TTuple& operator=(TTuple&& Other) noexcept
		{
			Move(std::move(Other), IndexSequence);
			return *this;
		}

		FORCEINLINE constexpr ~TTuple()
		{
			Deinit(IndexSequence);
		}

	private:
		template<size_t... Indices>
		NODISCARD FORCEINLINE constexpr bool Compare(const TTuple& Other, TIndexSequence<Indices...>) const
		{
			return (((*(ElementTypes*)(Storage + MembersInfo[Indices].Offset))
				== (*(ElementTypes*)(Other.Storage + MembersInfo[Indices].Offset))) && ...);
		}

	public:
		NODISCARD FORCEINLINE constexpr bool operator==(const TTuple& Other) const
		{
			return Compare(Other, IndexSequence);
		}

		NODISCARD FORCEINLINE constexpr bool operator!=(const TTuple& Other) const
		{
			return !Compare(Other, IndexSequence);
		}

		NODISCARD FORCEINLINE constexpr size_t GetSize() const noexcept
		{
			return sizeof...(ElementTypes);
		}

	private:
		template<size_t Index>
		using GetTypeByIndex = std::decay_t<decltype(std::get<Index>(std::tuple<ElementTypes...>()))>;

	public:
		template<size_t Index>
		constexpr auto& Get()&
		{
			static_assert(Index >= 0 && Index < sizeof...(ElementTypes), "TTuple::Get: Index out of bound");
			return *(GetTypeByIndex<Index>*)(Storage + MembersInfo[Index].Offset);
		}

		template<size_t Index>
		constexpr const auto& Get() const&
		{
			return const_cast<TTuple*>(this)->Get<Index>();
		}

		template<size_t Index>
		constexpr auto Get() const&&
		{
			return const_cast<TTuple*>(this)->Get<Index>();
		}

	private:
		using FTypeList = TTypeList<ElementTypes...>;

		template<typename ElementType>
		static constexpr size_t Occurrences = FTypeList::template Occurrences<ElementType>();

		template<typename ElementType>
		static constexpr bool IsUnique = FTypeList::template IsUnique<ElementType>();

		template<typename ElementType>
		static constexpr size_t IndexOf = FTypeList::template GetIndex<ElementType>();

	public:
		template<typename ElementTypeToGet>
		NODISCARD constexpr auto& Get()&
		{
			static_assert(Occurrences<ElementTypeToGet>, "TTuple::Get: Trying to get not stored element");
			static_assert(IsUnique<ElementTypeToGet>, "TTuple::Get: Ambiguous call, there is multiple variables with this type");
			const size_t Index = IndexOf<ElementTypeToGet>;
			return *(GetTypeByIndex<Index>*)(Storage + MembersInfo[Index].Offset);
		}

		template<typename ElementTypeToGet>
		NODISCARD FORCEINLINE constexpr const auto& Get() const&
		{
			return const_cast<TTuple*>(this)->Get<ElementTypeToGet>();
		}

		template<typename ElementTypeToGet>
		NODISCARD FORCEINLINE constexpr auto Get() const&&
		{
			return const_cast<TTuple*>(this)->Get<ElementTypeToGet>();
		}

		template<typename FunctionType>
		FORCEINLINE constexpr void ForEach(FunctionType&& Visitor)
		{
			ForEachImpl(std::forward<FunctionType>(Visitor), IndexSequence);
		}

		template<typename FunctionType>
		FORCEINLINE constexpr void ForEach(FunctionType&& Visitor) const
		{
			const_cast<TTuple*>(this)->ForEach(std::forward<FunctionType>(Visitor));
		}

	private:
		struct FMemberInfo
		{
			size_t Size;
			size_t Offset;
		};

		alignas(ElementTypes...) std::byte Storage[(sizeof(ElementTypes) + ...)];

		static constexpr std::array<FMemberInfo, sizeof...(ElementTypes)> MembersInfo = []() -> std::array<FMemberInfo, sizeof...(ElementTypes)>
		{
			std::array<FMemberInfo, sizeof...(ElementTypes)> Result =
			{
				FMemberInfo{sizeof(ElementTypes), 0}...
			};
			size_t Offset = 0;
			for (FMemberInfo& MemberInfo : Result)
			{
				MemberInfo.Offset = Offset;
				Offset += MemberInfo.Size;
			}
			return Result;
		}();

		template<size_t... Indices>
		constexpr void Init(TIndexSequence<Indices...>, ElementTypes&&... Args)
		{
			(new (Storage + MembersInfo[Indices].Offset) ElementTypes(std::forward<ElementTypes>(Args)), ...);
		}

		template<typename ElementType>
		FORCEINLINE constexpr void Destroy(const size_t Offset)
		{
			if constexpr (!std::is_trivially_destructible_v<ElementType>)
			{
				((ElementType*)(Storage + Offset))->~ElementType();
			}
		}

		template<size_t... Indices>
		constexpr void Deinit(TIndexSequence<Indices...>)
		{
			(Destroy<ElementTypes>(MembersInfo[Indices].Offset), ...);
		}

		template<typename FunctionType, size_t... Indices>
		constexpr void ForEachImpl(FunctionType&& Visitor, TIndexSequence<Indices...>)
		{
			(Visitor(Get<Indices>()), ...);
		}

		template<typename FunctionType, size_t... Indices>
		constexpr void ForEachImpl(FunctionType&& Visitor, TIndexSequence<Indices...>) const
		{
			(Visitor(Get<Indices>()), ...);
		}
	};

	template<> class TTuple<> {};

	template<typename ElementType>
	class TTuple<ElementType>
	{
	public:
		template<typename ArgType>
			requires std::is_convertible_v<ArgType&&, ElementType>
		constexpr TTuple(ArgType&& Arg)
			: First(std::forward<ArgType>(Arg))
		{
		}

		constexpr TTuple() = default;
		constexpr TTuple(TTuple&&) = default;
		constexpr TTuple(const TTuple&) = default;
		constexpr TTuple& operator=(TTuple&&) = default;
		constexpr TTuple& operator=(const TTuple&) = default;

		NODISCARD FORCEINLINE constexpr bool operator==(const TTuple& Other) const
		{
			return First == Other.First;
		}

		NODISCARD FORCEINLINE constexpr bool operator!=(const TTuple& Other) const
		{
			return First != Other.First;
		}

		ElementType First;

		NODISCARD FORCEINLINE constexpr size_t GetSize() const noexcept
		{
			return 1;
		}

	private:
		template<size_t Index>
		using GetTypeByIndex = ElementType;

	public:
		template<size_t Index>
		constexpr ElementType& Get()&
		{
			static_assert(Index == 0, "TTuple::Get: Index out of bound");
			return First;
		}

		template<size_t Index>
		constexpr const ElementType& Get() const&
		{
			static_assert(Index == 0, "TTuple::Get: Index out of bound");
			return First;
		}

		template<size_t Index>
		constexpr ElementType Get() const&&
		{
			static_assert(Index == 0, "TTuple::Get: Index out of bound");
			return First;
		}

		template<typename ElementTypeToGet>
		constexpr ElementType& Get()&
		{
			static_assert(std::is_same_v<ElementType, ElementTypeToGet>, "TTuple::Get: Trying to get not stored element");
			return First;
		}

		template<typename ElementTypeToGet>
		constexpr const ElementType& Get() const&
		{
			static_assert(std::is_same_v<ElementType, ElementTypeToGet>, "TTuple::Get: Trying to get not stored element");
			return First;
		}

		template<typename ElementTypeToGet>
		constexpr ElementType Get() const&&
		{
			static_assert(std::is_same_v<ElementType, ElementTypeToGet>, "TTuple::Get: Trying to get not stored element");
			return First;
		}

		template<typename FunctionType>
		constexpr void ForEach(FunctionType&& Visitor)&
		{
			Visitor(First);
		}

		template<typename FunctionType>
		constexpr void ForEach(FunctionType&& Visitor) const&
		{
			Visitor(First);
		}

		template<typename FunctionType>
		constexpr void ForEach(FunctionType&& Visitor)&&
		{
			Visitor(std::move(First));
		}

		template<typename FunctionType>
		constexpr void ForEach(FunctionType&& Visitor) const&&
		{
			Visitor(std::move(First));
		}
	};

	template<typename FirstElementType, typename SecondElementType>
	class TTuple<FirstElementType, SecondElementType>
	{
	public:
		template<typename FirstArgType, typename SecondArgType>
			requires std::conjunction_v<std::is_convertible<FirstArgType&&, FirstElementType>, std::is_convertible<SecondArgType&&, SecondElementType>>
		constexpr TTuple(FirstArgType&& First, SecondArgType&& Second)
			: First(std::forward<FirstArgType>(First))
			, Second(std::forward<SecondArgType>(Second))
		{
		}

		constexpr TTuple() = default;
		constexpr TTuple(TTuple&&) = default;
		constexpr TTuple(const TTuple&) = default;
		constexpr TTuple& operator=(TTuple&&) = default;
		constexpr TTuple& operator=(const TTuple&) = default;

		NODISCARD FORCEINLINE constexpr bool operator==(const TTuple& Other) const
		{
			return First == Other.First && Second == Other.Second;
		}

		NODISCARD FORCEINLINE constexpr bool operator!=(const TTuple& Other) const
		{
			return First != Other.First && Second != Other.Second;
		}

		FirstElementType First;
		SecondElementType Second;

		NODISCARD FORCEINLINE constexpr size_t GetSize() const noexcept
		{
			return 2;
		}

	private:
		template<size_t Index>
		constexpr auto& GetImpl() const
		{
			if constexpr (Index == 0)
			{
				return First;
			}
			else
			{
				return Second;
			}
		}

	public:
		template<size_t Index>
		constexpr auto& Get()&
		{
			static_assert(Index >= 0 && Index < 2, "TTuple::Get: Index out of bound");
			return GetImpl<Index>();
		}

		template<size_t Index>
		constexpr const auto& Get() const&
		{
			static_assert(Index >= 0 && Index < 2, "TTuple::Get: Index out of bound");
			return GetImpl<Index>();
		}

		template<size_t Index>
		constexpr auto Get() const&&
		{
			static_assert(Index >= 0 && Index < 2, "TTuple::Get: Index out of bound");
			return GetImpl<Index>();
		}

	private:
		using FTypeList = TTypeList<FirstElementType, SecondElementType>;

		template<typename ElementType>
		static constexpr size_t Occurrences = FTypeList::template Occurrences<ElementType>();

		template<typename ElementType>
		static constexpr bool IsUnique = FTypeList::template IsUnique<ElementType>();

		template<typename ElementType>
		static constexpr size_t IndexOf = FTypeList::template GetIndex<ElementType>();

	public:
		template<typename ElementTypeToGet>
		constexpr auto& Get()&
		{
			static_assert(Occurrences<ElementTypeToGet>, "TTuple::Get: Trying to get not stored element");
			static_assert(IsUnique<ElementTypeToGet>, "TTuple::Get: Ambiguous call, there is multiple variables with this type");
			const size_t Index = IndexOf<ElementTypeToGet>;
			return GetImpl<Index>();
		}

		template<typename ElementTypeToGet>
		constexpr const auto& Get() const&
		{
			static_assert(Occurrences<ElementTypeToGet>, "TTuple::Get: Trying to get not stored element");
			static_assert(IsUnique<ElementTypeToGet>, "TTuple::Get: Ambiguous call, there is multiple variables with this type");
			const size_t Index = IndexOf<ElementTypeToGet>;
			return GetImpl<Index>();
		}

		template<typename ElementTypeToGet>
		constexpr auto Get() const&&
		{
			static_assert(Occurrences<ElementTypeToGet>, "TTuple::Get: Trying to get not stored element");
			static_assert(IsUnique<ElementTypeToGet>, "TTuple::Get: Ambiguous call, there is multiple variables with this type");
			const size_t Index = IndexOf<ElementTypeToGet>;
			return GetImpl<Index>();
		}

		template<typename FunctionType>
		constexpr void ForEach(FunctionType&& Visitor)&
		{
			Visitor(First);
			Visitor(Second);
		}

		template<typename FunctionType>
		constexpr void ForEach(FunctionType&& Visitor) const&
		{
			Visitor(First);
			Visitor(Second);
		}

		template<typename FunctionType>
		constexpr void ForEach(FunctionType&& Visitor)&&
		{
			Visitor(std::move(First));
			Visitor(std::move(Second));
		}

		template<typename FunctionType>
		constexpr void ForEach(FunctionType&& Visitor) const&&
		{
			Visitor(std::move(First));
			Visitor(std::move(Second));
		}
	};
}
