#pragma once

#include "Platform/Platform.h"
#include "Templates/TypeTraits.h"

namespace Bloodshot
{
	template<typename ElementType>
	FORCEINLINE void DestructElement(ElementType* const Element)
	{
		if constexpr (!std::is_trivially_destructible_v<ElementType>)
		{
			Element->~ElementType();
		}
	}

	template<typename ElementType>
	FORCEINLINE void DefaultConstructElements(void* const Elements, const size_t Count)
	{
		if (!Count) return;

		if constexpr (TIsZeroConstructibleValue<ElementType>)
		{
			memset(Elements, 0, sizeof(ElementType) * Count);
		}
		else
		{
			for (size_t i = 0; i < Count; ++i)
			{
				new((ElementType*)Elements + i) ElementType;
			}
		}
	}

	template<typename DestinationElementType, typename SourceElementType>
	FORCEINLINE void ConstructElements(void* const Destination, const SourceElementType* const Source, const size_t Count)
	{
		if (!Count) return;

		for (size_t i = 0; i < Count; ++i)
		{
			new((DestinationElementType*)Destination + i) DestinationElementType(Source[i]);
		}
	}

	template<typename ElementType>
	FORCEINLINE void MoveConstructElements(void* const Destination, ElementType* const Source, const size_t Count)
	{
		if (!Count) return;

		if constexpr (std::is_trivially_copy_constructible_v<ElementType>)
		{
			memmove(Destination, Source, sizeof(ElementType) * Count);
		}
		else
		{
			for (size_t i = 0; i < Count; ++i)
			{
				new((ElementType*)Destination + i) ElementType(std::move(Source[i]));
			}
		}
	}

	template<typename ElementType>
	FORCEINLINE void CopyAssignElements(ElementType* const Destination, const ElementType* const Source, const size_t Count)
	{
		if (!Count) return;

		if constexpr (std::is_trivially_copy_assignable_v<ElementType>)
		{
			memmove(Destination, Source, sizeof(ElementType) * Count);
		}
		else
		{
			for (size_t i = 0; i < Count; ++i)
			{
				Destination[i] = Source[i];
			}
		}
	}

	template<typename ElementType>
	FORCEINLINE void MoveAssignElements(ElementType* const Destination, ElementType* const Source, const size_t Count)
	{
		if (!Count) return;

		if constexpr (std::is_trivially_copy_assignable_v<ElementType>)
		{
			memmove(Destination, Source, sizeof(ElementType) * Count);
		}
		else
		{
			for (size_t i = 0; i < Count; ++i)
			{
				*(Destination + i) = std::move(Source[i]);
			}
		}
	}

	template<typename ElementType>
	FORCEINLINE void DestructElements(ElementType* const Elements, const size_t Count)
	{
		if (!Count) return;

		if constexpr (!std::is_trivially_destructible_v<ElementType>)
		{
			for (size_t i = 0; i < Count; ++i)
			{
				(Elements + i)->~ElementType();
			}
		}
	}
}
