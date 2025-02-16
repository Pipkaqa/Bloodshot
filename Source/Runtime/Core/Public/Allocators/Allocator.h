#pragma once

#include "Misc/AssertionMacros.h"
#include "Platform/Platform.h"
#include "Templates/TypeTraits.h"

#include <utility>

namespace Bloodshot
{
	class IAllocator
	{
	public:
		virtual ~IAllocator() = default;

		virtual void* Allocate(const size_t Count) = 0;
		virtual void Deallocate(void* const Block, const size_t Size) = 0;

		virtual void Reset() = 0;
		virtual void Dispose() = 0;

		NODISCARD FORCEINLINE size_t GetAllocatedSize() const noexcept
		{
			return AllocatedSize;
		}

		NODISCARD FORCEINLINE size_t GetDeallocatedSize() const noexcept
		{
			return DeallocatedSize;
		}
		
	protected:
		FORCEINLINE void OnAllocate(const size_t Size) noexcept
		{
			AllocatedSize += Size;
		}

		FORCEINLINE void OnDeallocate(const size_t Size) noexcept
		{
			DeallocatedSize += Size;
		}

	private:
		size_t AllocatedSize = 0;
		size_t DeallocatedSize = 0;
	};

	template<typename T>
	concept IsAllocator = std::is_base_of_v<IAllocator, T>;

	template<typename InElementType>
	class IAllocatorBase : public IAllocator
	{
	public:
		using ElementType = InElementType;
		using PointerType = ElementType*;
		using ConstPointerType = const ElementType*;
		using ReferenceType = ElementType&;
		using ConstReferenceType = const ElementType&;
	};

	template<typename InElementType>
	class TAllocator final : public IAllocatorBase<InElementType>
	{
	public:
		using Super = IAllocatorBase<InElementType>;
		using ElementType = Super::ElementType;

		template<typename OtherElementType>
		using Rebind = TAllocator<OtherElementType>;

		virtual void* Allocate(const size_t Count) override
		{
			static_assert(sizeof(ElementType) > 0, "ElementType must be complete before calling Allocate");
			const size_t SizeToAllocate = Count * sizeof(ElementType);
			void* const Result = ::operator new(SizeToAllocate);
			IAllocator::OnAllocate(SizeToAllocate);
			return Result;
		}

		virtual void Deallocate(void* const Block, const size_t Size) override
		{
			::operator delete(Block, Size);
			IAllocator::OnDeallocate(Size);
		}

		virtual void Reset() override {}
		virtual void Dispose() override {}
	};

	template<IsAllocator InAllocatorType>
	class TAllocatorSharedWrapper final : public IAllocatorBase<typename InAllocatorType::ElementType>
	{
	public:
		using ElementType = typename InAllocatorType::ElementType;
		using Super = IAllocatorBase<ElementType>;
		using AllocatorType = InAllocatorType;

		template<typename... ArgTypes>
		FORCEINLINE TAllocatorSharedWrapper(ArgTypes&&... Args)
			: Allocator(new AllocatorType(std::forward<ArgTypes>(Args)...))
		{
		}

		FORCEINLINE TAllocatorSharedWrapper(const TAllocatorSharedWrapper& Other)
			: Allocator(Other.Allocator)
			, bOwnAllocator(false)
		{
		}

		FORCEINLINE TAllocatorSharedWrapper(TAllocatorSharedWrapper&& Other) noexcept
			: Allocator(std::exchange(Other.Allocator, nullptr))
			, bOwnAllocator(std::exchange(Other.bOwnAllocator, false))
		{
		}

		virtual ~TAllocatorSharedWrapper() override
		{
			if (bOwnAllocator)
			{
				delete Allocator;
			}
		}

		FORCEINLINE TAllocatorSharedWrapper& operator=(const TAllocatorSharedWrapper& Other)
		{
			Allocator = Other.Allocator;
			bOwnAllocator = Other.bOwnAllocator;
			return *this;
		}

		FORCEINLINE TAllocatorSharedWrapper& operator=(TAllocatorSharedWrapper&& Other) noexcept
		{
			Allocator = std::exchange(Other.Allocator, nullptr);
			bOwnAllocator = std::exchange(Other.bOwnAllocator, false);
			return *this;
		}

		virtual void* Allocate(const size_t Count) override
		{
			BS_CHECK(Allocator);
			return Allocator->Allocate(Count);
		}

		virtual void Deallocate(void* const Block, const size_t Size) override
		{
			if (!Allocator) return;
			return Allocator->Deallocate(Block, Size);
		}

		virtual void Reset() override
		{
			if (!Allocator) return;
			return Allocator->Reset();
		}

		virtual void Dispose() override
		{
			if (!Allocator) return;
			return Allocator->Dispose();
		}

	private:
		AllocatorType* Allocator = nullptr;
		bool bOwnAllocator = true;
	};
}
