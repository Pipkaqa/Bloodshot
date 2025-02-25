#pragma once

#include "Misc/Casts.h"
#include "Platform/Platform.h"

#include <memory>

namespace Bloodshot
{
	template<typename ElementType>
	using TSharedPtr = std::shared_ptr<ElementType>;

	template<typename ElementType, typename... ArgTypes>
	NODISCARD FORCEINLINE constexpr TSharedPtr<ElementType> MakeShared(ArgTypes&&... Args)
	{
		return std::make_shared<ElementType>(std::forward<ArgTypes>(Args)...);
	}

	template<typename ElementType, typename... ArgTypes>
	NODISCARD FORCEINLINE constexpr TSharedPtr<ElementType> MakeShared(ElementType* const Src)
	{
		return std::shared_ptr<ElementType>(Src);
	}

	template<typename InElementType>
	class TReference
	{
		template<typename OtherElementType>
		friend class TReference;

	public:
		using ElementType = InElementType;
		using PointerType = ElementType*;
		using ConstPointerType = const ElementType*;
		using ReferenceType = ElementType&;
		using ConstReferenceType = const ElementType&;

		FORCEINLINE TReference() = default;

		FORCEINLINE TReference(const TReference& Other)
			: Ptr(Other.Ptr)
		{
		}

		FORCEINLINE TReference(TReference&& Other) noexcept
			: Ptr(Other.Ptr)
		{
			Other.Ptr = nullptr;
		}

		template<typename OtherType>
		FORCEINLINE TReference(const TReference<OtherType>& Other)
			: Ptr(Other.Ptr)
		{
		}

		template<typename OtherType>
		FORCEINLINE TReference(TReference<OtherType>&& Other) noexcept
			: Ptr(Other.Ptr)
		{
			Other.Ptr = nullptr;
		}

		FORCEINLINE explicit TReference(std::nullptr_t Nullptr) noexcept
			: Ptr(Nullptr)
		{
		}

		FORCEINLINE TReference(PointerType Object) noexcept
			: Ptr(Object)
		{
		}

		FORCEINLINE ~TReference() {}

		FORCEINLINE TReference& operator=(const TReference& Other) noexcept
		{
			Ptr = Other.Ptr;
			return *this;
		}

		template<typename OtherType>
		FORCEINLINE TReference& operator=(const TReference<OtherType>& Other) noexcept
		{
			Ptr = Other.Ptr;
			return *this;
		}

		FORCEINLINE TReference& operator=(TReference&& Other) noexcept
		{
			Ptr = Other.Ptr;
			Other.Ptr = nullptr;
			return *this;
		}

		template<typename OtherType>
		FORCEINLINE TReference& operator=(TReference<OtherType>&& Other) noexcept
		{
			Ptr = Other.Ptr;
			Other.Ptr = nullptr;
			return *this;
		}

		FORCEINLINE TReference& operator=(std::nullptr_t Nullptr) noexcept
		{
			Ptr = Nullptr;
			return *this;
		}

		FORCEINLINE TReference& operator=(PointerType Object) noexcept
		{
			Ptr = Object;
			return *this;
		}

		FORCEINLINE ReferenceType operator*()
		{
			return *Ptr;
		}

		FORCEINLINE ConstReferenceType operator*() const
		{
			return *Ptr;
		}

		PointerType operator->()
		{
			return Ptr;
		}

		ConstPointerType operator->() const
		{
			return Ptr;
		}

		FORCEINLINE bool operator==(std::nullptr_t Nullptr) const noexcept
		{
			return Ptr == Nullptr;
		}

		FORCEINLINE bool operator!=(std::nullptr_t Nullptr) const noexcept
		{
			return !(Ptr == Nullptr);
		}

		NODISCARD FORCEINLINE operator bool() const noexcept
		{
			return IsValid();
		}

		NODISCARD FORCEINLINE bool IsValid() const noexcept
		{
			return Ptr != nullptr;
		}

		NODISCARD FORCEINLINE PointerType GetRawPtr() noexcept
		{
			return Ptr;
		}

		NODISCARD FORCEINLINE ConstPointerType GetRawPtr() const noexcept
		{
			return Ptr;
		}

		template<typename T>
		NODISCARD FORCEINLINE TReference<T> As()
		{
			return ReinterpretCast<T*>(Ptr);
		}

		template<typename T>
		NODISCARD FORCEINLINE const TReference<T> As() const
		{
			return ReinterpretCast<T*>(Ptr);
		}

	private:
		PointerType Ptr = nullptr;
	};

	template<typename T>
	NODISCARD FORCEINLINE bool operator==(std::nullptr_t Nullptr, const TReference<T>& Other)
	{
		return Other.GetRawPtr() == nullptr;
	}

	template<typename T>
	NODISCARD FORCEINLINE bool operator!=(std::nullptr_t Nullptr, const TReference<T>& Other)
	{
		return !(Other.GetRawPtr() == nullptr);
	}

	template<typename T>
	using TDefaultDeleter = std::default_delete<T>;

	template<typename InElementType, typename InDeleterType = TDefaultDeleter<InElementType>>
	class TUniquePtr
	{
		template<typename OtherType, typename OtherDeleterType>
		friend class TUniquePtr;

	public:
		using ElementType = InElementType;
		using DeleterType = InDeleterType;
		using PointerType = ElementType*;
		using ConstPointerType = const ElementType*;
		using ReferenceType = ElementType&;
		using ConstReferenceType = const ElementType&;

		FORCEINLINE TUniquePtr() = default;
		TUniquePtr(const TUniquePtr& Other) = delete;

		FORCEINLINE TUniquePtr(TUniquePtr&& Other) noexcept
			: Ptr(Other.Ptr)
			, Deleter(std::move(Other.Deleter))
		{
			Other.Ptr = nullptr;
		}

		FORCEINLINE ~TUniquePtr()
		{
			Reset();
		}

		FORCEINLINE TUniquePtr(std::nullptr_t Nullptr) noexcept : Ptr(Nullptr) {}
		FORCEINLINE explicit TUniquePtr(PointerType Object) noexcept : Ptr(Object) {}

		TUniquePtr& operator=(const TUniquePtr& Other) = delete;

		FORCEINLINE TUniquePtr& operator=(TUniquePtr&& Other) noexcept
		{
			if (this == &Other) return *this;

			PointerType OldPtr = Ptr;
			Ptr = Other.Ptr;
			Other.Ptr = nullptr;
			Deleter = std::move(Other.Deleter);
			return *this;
		}

		template<typename OtherType, typename OtherDeleterType>
		FORCEINLINE TUniquePtr& operator=(TUniquePtr<OtherType, OtherDeleterType>&& Other) noexcept
		{
			PointerType OldPtr = Ptr;
			Ptr = Other.Ptr;
			Other.Ptr = nullptr;
			Deleter = std::move(Other.Deleter);
			return *this;
		}

		FORCEINLINE TUniquePtr& operator=(std::nullptr_t Nullptr) noexcept
		{
			Ptr = Nullptr;
			return *this;
		}

		FORCEINLINE TUniquePtr& operator=(PointerType Object) noexcept
		{
			Ptr = Object;
			return *this;
		}

		FORCEINLINE ReferenceType operator*()
		{
			return *Ptr;
		}

		FORCEINLINE ConstReferenceType operator*() const
		{
			return *Ptr;
		}

		PointerType operator->()
		{
			return Ptr;
		}

		ConstPointerType operator->() const
		{
			return Ptr;
		}

		FORCEINLINE bool operator==(std::nullptr_t Nullptr)
		{
			return Ptr == Nullptr;
		}

		FORCEINLINE bool operator!=(std::nullptr_t Nullptr)
		{
			return !(Ptr == Nullptr);
		}

		NODISCARD FORCEINLINE operator bool() const noexcept
		{
			return IsValid();
		}

		NODISCARD FORCEINLINE bool IsValid() const noexcept
		{
			return Ptr != nullptr;
		}

		NODISCARD FORCEINLINE PointerType GetRawPtr() noexcept
		{
			return Ptr;
		}

		NODISCARD FORCEINLINE ConstPointerType GetRawPtr() const noexcept
		{
			return Ptr;
		}

		NODISCARD FORCEINLINE TReference<ElementType> GetReference()
		{
			return Ptr;
		}

		NODISCARD FORCEINLINE const TReference<ElementType> GetReference() const
		{
			return Ptr;
		}

		NODISCARD FORCEINLINE DeleterType& GetDeleter()
		{
			return Deleter;
		}

		NODISCARD FORCEINLINE const DeleterType& GetDeleter() const
		{
			return Deleter;
		}

		NODISCARD FORCEINLINE PointerType Release()
		{
			PointerType Result = Ptr;
			Ptr = nullptr;
			return Result;
		}

		FORCEINLINE void Reset(PointerType InPtr)
		{
			PointerType OldPtr = Ptr;
			Ptr = InPtr;
			Deleter(OldPtr);
		}

		FORCEINLINE void Reset(std::nullptr_t InPtr = nullptr)
		{
			PointerType OldPtr = Ptr;
			Ptr = InPtr;
			Deleter(OldPtr);
		}

	private:
		PointerType Ptr = nullptr;
		DeleterType Deleter{};
	};

	template<typename T>
	NODISCARD FORCEINLINE bool operator==(std::nullptr_t Nullptr, const TUniquePtr<T>& Other)
	{
		return Other.GetRawPtr() == nullptr;
	}

	template<typename T>
	NODISCARD FORCEINLINE bool operator!=(std::nullptr_t Nullptr, const TUniquePtr<T>& Other)
	{
		return !(Other.GetRawPtr() == nullptr);
	}

	template<typename T, typename... ArgTypes>
	NODISCARD FORCEINLINE TUniquePtr<T> MakeUnique(ArgTypes&&... Args)
	{
		return TUniquePtr<T>(new T(std::forward<ArgTypes>()...));
	}
}
