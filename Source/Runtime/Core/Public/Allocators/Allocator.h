#pragma once

#include "Memory/Memory.h"
#include "Misc/AssertionMacros.h"
#include "Platform/Platform.h"
#include "Templates/TypeTraits.h"

#include <limits>
#include <utility>

namespace Bloodshot
{
	class IAllocator
	{
	public:
		class Contiguous final
		{
		public:
			class ForAnyElementType {};
		};

		class NonContiguous final
		{
		public:
			class ForAnyElementType {};
		};
	};

	template<typename T>
	concept IsNonSpecializedAllocator = std::is_base_of_v<IAllocator, T>;

	template<typename T>
	concept IsContiguousAllocator = std::is_base_of_v<IAllocator::Contiguous::ForAnyElementType, T>;

	template<typename T>
	concept IsNonContiguousAllocator = std::is_base_of_v<IAllocator::NonContiguous::ForAnyElementType, T>;

	template<typename T>
	concept IsSpecializedAllocator = IsContiguousAllocator<T> || IsNonContiguousAllocator<T>;

	template<typename T>
	concept IsAllocator = IsNonSpecializedAllocator<T> || IsSpecializedAllocator<T>;

	class FDefaultAllocator final : public IAllocator
	{
	public:
		class ForAnyElementType : public IAllocator::Contiguous::ForAnyElementType
		{
		public:
			FORCEINLINE ~ForAnyElementType()
			{
				Dispose();
			}

			NODISCARD FORCEINLINE void* GetAllocation() const noexcept
			{
				return Data;
			}

			NODISCARD FORCEINLINE size_t GetInitialCapacity() const noexcept
			{
				return 0;
			}

			NODISCARD FORCEINLINE bool HasAllocation() const noexcept
			{
				return !!Data;
			}

			FORCEINLINE void Move(ForAnyElementType& Other) noexcept
			{
				BS_CHECK(this != &Other);
				if (Data)
				{
					FMemory::Free(Data);
				}
				Data = Other.Data;
				Other.Data = nullptr;
			}

			NODISCARD FORCEINLINE void* ExchangeAllocation(const size_t CurrentSize, const size_t NewSize, const size_t ElementSizeInBytes)
			{
				void* const OldData = Data;
				Data = nullptr;
				ResizeAllocation(CurrentSize, NewSize, ElementSizeInBytes);
				return OldData;
			}

			FORCEINLINE void ConsumeAllocation(void* const InData) const noexcept
			{
				FMemory::Free(InData);
			}

			FORCEINLINE void ResizeAllocation(const size_t CurrentSize, const size_t NewSize, const size_t ElementSizeInBytes)
			{
				if (NewSize)
				{
					const bool bInvalidResize = !ElementSizeInBytes
						|| ElementSizeInBytes > std::numeric_limits<int32_t>::max()
						|| NewSize > std::numeric_limits<size_t>::max() / ElementSizeInBytes;
					BS_ASSERT(!bInvalidResize, "Invalid heap allocation: CurrentSize: {}, NewSize: {}, ElementSizeInBytes: {}", CurrentSize, NewSize, ElementSizeInBytes);
					Data = FMemory::Realloc(Data, NewSize * ElementSizeInBytes);
				}
			}

			FORCEINLINE void Dispose() noexcept
			{
				FMemory::Free(Data);
				Data = nullptr;
			}

		private:
			void* Data = nullptr;
		};

		template<typename ElementType>
		class ForElementType : public ForAnyElementType
		{
			using Super = ForAnyElementType;
			using Super::GetAllocation;
			using Super::ExchangeAllocation;

		public:
			NODISCARD FORCEINLINE ElementType* GetAllocation() const noexcept
			{
				return reinterpret_cast<ElementType*>(Super::GetAllocation());
			}

			NODISCARD FORCEINLINE ElementType* ExchangeAllocation(const size_t CurrentSize, const size_t NewSize, const size_t ElementSizeInBytes)
			{
				return reinterpret_cast<ElementType*>(Super::ExchangeAllocation(CurrentSize, NewSize, ElementSizeInBytes));
			}
		};
	};

	template<size_t InSize>
	class TInlineAllocator final : public IAllocator
	{
	public:
		class ForAnyElementType : public IAllocator::Contiguous::ForAnyElementType
		{
		public:
			FORCEINLINE ~ForAnyElementType()
			{
				Dispose();
			}

			NODISCARD FORCEINLINE void* GetAllocation() const noexcept
			{
				return InlineData;
			}

			NODISCARD FORCEINLINE size_t GetInitialCapacity() const noexcept
			{
				return InSize;
			}

			NODISCARD FORCEINLINE bool HasAllocation() const noexcept
			{
				return true;
			}

			NODISCARD FORCEINLINE void* ExchangeAllocation(const size_t CurrentSize, const size_t NewSize, const size_t ElementSizeInBytes)
			{
				return nullptr;
			}

			FORCEINLINE void ConsumeAllocation(void* const InData) const noexcept
			{
			}

			FORCEINLINE void ResizeAllocation(const size_t CurrentSize, const size_t NewSize, const size_t ElementSizeInBytes)
			{
			}

			FORCEINLINE void Dispose() noexcept
			{
			}

		private:
			std::byte InlineData[InSize];
		};

		template<typename ElementType>
		class ForElementType : public ForAnyElementType
		{
			using Super = ForAnyElementType;
			using Super::GetAllocation;
			using Super::ExchangeAllocation;

		public:
			NODISCARD FORCEINLINE ElementType* GetAllocation() const noexcept
			{
				return reinterpret_cast<ElementType*>(Super::GetAllocation());
			}

			NODISCARD FORCEINLINE ElementType* ExchangeAllocation(const size_t CurrentSize, const size_t NewSize, const size_t ElementSizeInBytes)
			{
				return reinterpret_cast<ElementType*>(Super::ExchangeAllocation(CurrentSize, NewSize, ElementSizeInBytes));
			}
		};
	};

	class FAllocatorView
	{
	public:
		class Contiguous
		{
		public:
			class Generic
			{
			public:
				template<IsContiguousAllocator AllocatorType>
				FORCEINLINE Generic(AllocatorType& InAllocator)
					: AllocatorPtr(&InAllocator)
					, GetAllocationFuncPtr([](void* const Allocator) -> void* { return ((AllocatorType*)Allocator)->GetAllocation(); })
					, ResizeAllocationFuncPtr([](void* const Allocator, const size_t CurrentSize, const size_t NewSize, const size_t ElementSizeInBytes) { ((AllocatorType*)Allocator)->ResizeAllocation(CurrentSize, NewSize, ElementSizeInBytes); })
					, DestructorFuncPtr([](void* const Allocator) { ((AllocatorType*)Allocator)->~AllocatorType(); })
				{
				}

				NODISCARD FORCEINLINE void* GetAllocatorPtr() const noexcept
				{
					return AllocatorPtr;
				}

				NODISCARD FORCEINLINE void* GetAllocation() const
				{
					return GetAllocationFuncPtr(AllocatorPtr);
				}

				FORCEINLINE void ResizeAllocation(const size_t CurrentSize, const size_t NewSize, const size_t ElementSizeInBytes) const
				{
					ResizeAllocationFuncPtr(AllocatorPtr, CurrentSize, NewSize, ElementSizeInBytes);
				}

				FORCEINLINE void CallDestructor() const
				{
					DestructorFuncPtr(AllocatorPtr);
				}

			private:
				void* AllocatorPtr;
				void* (*GetAllocationFuncPtr)(void* const);
				void(*ResizeAllocationFuncPtr)(void* const, const size_t, const size_t, const size_t);
				void(*DestructorFuncPtr)(void* const);
			};

			class Typed
			{
			public:
				template<IsContiguousAllocator AllocatorType>
				FORCEINLINE Typed(AllocatorType& InAllocator)
					: AllocatorPtr(&InAllocator)
					, GetAllocationFuncPtr([](void* const Allocator) -> void* { return ((AllocatorType*)Allocator)->GetAllocation(); })
					, ResizeAllocationFuncPtr([](void* const Allocator, const size_t CurrentSize, const size_t NewSize) { ((AllocatorType*)Allocator)->ResizeAllocation(CurrentSize, NewSize); })
					, DestructorFuncPtr([](void* const Allocator) { ((AllocatorType*)Allocator)->~AllocatorType(); })
				{
				}

				NODISCARD FORCEINLINE void* GetAllocatorPtr() const noexcept
				{
					return AllocatorPtr;
				}

				NODISCARD FORCEINLINE void* GetAllocation() const
				{
					return GetAllocationFuncPtr(AllocatorPtr);
				}

				FORCEINLINE void ResizeAllocation(const size_t CurrentSize, const size_t NewSize) const
				{
					ResizeAllocationFuncPtr(AllocatorPtr, CurrentSize, NewSize);
				}

				FORCEINLINE void CallDestructor() const
				{
					DestructorFuncPtr(AllocatorPtr);
				}

			private:
				void* AllocatorPtr;
				void* (*GetAllocationFuncPtr)(void* const);
				void(*ResizeAllocationFuncPtr)(void* const, const size_t, const size_t);
				void(*DestructorFuncPtr)(void* const);
			};
		};

		class NonContiguous
		{
		public:
			class Generic
			{
			public:
				template<IsSpecializedAllocator AllocatorType>
				FORCEINLINE Generic(AllocatorType& InAllocator)
					: AllocatorPtr(&InAllocator)
					, AllocateFuncPtr([](void* const Allocator) -> void* { return ((AllocatorType*)Allocator)->Allocate(); })
					, DeallocateFuncPtr([](void* const Allocator, void* const Block) { ((AllocatorType*)Allocator)->Deallocate(Block); })
					, DestructorFuncPtr([](void* const Allocator) { ((AllocatorType*)Allocator)->~AllocatorType(); })
				{
				}

				NODISCARD FORCEINLINE void* GetAllocatorPtr()
				{
					return AllocatorPtr;
				}

				NODISCARD FORCEINLINE void* Allocate()
				{
					return AllocateFuncPtr(AllocatorPtr);
				}

				FORCEINLINE void Deallocate(void* const Block)
				{
					DeallocateFuncPtr(AllocatorPtr, Block);
				}

				FORCEINLINE void CallDestructor()
				{
					DestructorFuncPtr(AllocatorPtr);
				}

			private:
				void* AllocatorPtr;
				void* (*AllocateFuncPtr)(void* const);
				void(*DeallocateFuncPtr)(void* const, void* const);
				void(*DestructorFuncPtr)(void* const);
			};
		};
	};
}
