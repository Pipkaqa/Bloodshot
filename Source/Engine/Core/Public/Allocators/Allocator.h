#pragma once

#include <type_traits>

namespace Bloodshot
{
	class IAllocator abstract
	{
	public:
		virtual ~IAllocator() = default;

		virtual void* Allocate(const size_t Count) = 0;
		virtual void Deallocate(void* const Block, const size_t Size) = 0;

		virtual void Reset() = 0;
		virtual void Dispose() = 0;
	};

	template<typename T>
	concept IsAllocator = std::is_base_of_v<IAllocator, T>;

	template<typename InElementType>
	class IAllocatorBase abstract : public IAllocator
	{
	public:
		using ElementType = InElementType;
		using PointerType = ElementType*;
		using ConstPointerType = const ElementType*;
		using ReferenceType = ElementType&;
		using ConstReferenceType = const ElementType&;
	};

	template<typename InElementType>
	class TAllocator : public IAllocatorBase<InElementType>
	{
	public:
		using Super = IAllocatorBase<InElementType>;
		using ElementType = Super::ElementType;

		virtual void* Allocate(const size_t Count) override
		{
			return ::operator new(Count * sizeof(ElementType));
		}

		virtual void Deallocate(void* const Block, const size_t Size) override
		{
			::operator delete(Block, Size);
		}

		virtual void Reset() override {}
		virtual void Dispose() override {}
	};
}
