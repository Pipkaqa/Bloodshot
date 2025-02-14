#pragma once

//#include "Allocators/Allocator.h"
//#include "Misc/AssertionMacros.h"
//#include "Misc/Casts.h"
//#include "Platform/Platform.h"
//#include "Templates/MemoryOperations.h"
//#include "Templates/TypeTraits.h"

#include <set>

namespace Bloodshot
{
	// BSTODO: Implement

	template<typename InElementType,
		typename InPredicateType = std::less<InElementType>,
		typename InAllocatorType = std::allocator<InElementType>>
	using TSet = std::set<InElementType, InPredicateType, InAllocatorType>;

	//namespace Private
	//{
	//	template<typename T>
	//	class TSetNode
	//	{
	//	public:
	//		T Data;
	//		TSetNode* Left;
	//		TSetNode* Right;
	//		uint32_t Height;
	//	};
	//}
	//
	//template<typename InElementType,
	//	typename InHasherType = std::hash<InElementType>,
	//	typename InPredicateType = std::less<InElementType>,
	//	IsAllocator InAllocatorType = TAllocator<Private::TSetNode<InElementType>>>
	//class TSet
	//{
	//public:
	//	using ElementType = InElementType;
	//	using PointerType = ElementType*;
	//	using ConstPointerType = const ElementType*;
	//	using ReferenceType = ElementType&;
	//	using ConstReferenceType = const ElementType&;
	//	using RValueReferenceType = ElementType&&;
	//
	//	using NodeType = Private::TSetNode<ElementType>;
	//	using HasherType = InHasherType;
	//	using PredicateType = InPredicateType;
	//	using AllocatorType = InAllocatorType;
	//
	//	//using FIterator = TIndexedContainerIterator<TArray, ElementType>;
	//	//using FConstIterator = TIndexedContainerIterator<const TArray, const ElementType>;
	//	//
	//	//using FRangeBasedForIteratorType = TCheckedPointerIterator<ElementType, false>;
	//	//using FRangeBasedForConstIteratorType = TCheckedPointerIterator<const ElementType, false>;
	//	//using FRangeBasedForReverseIteratorType = TCheckedPointerIterator<ElementType, true>;
	//	//using FRangeBasedForConstReverseIteratorType = TCheckedPointerIterator<const ElementType, true>;
	//
	//	void* Insert(ConstReferenceType Element)
	//	{
	//		const size_t Index = Hasher(Element) % 
	//
	//		if (!Root)
	//		{
	//			Root =
	//		}
	//	}
	//
	//	//void* Insert(RValueReferenceType Element)
	//	//{
	//	//	
	//	//}
	//
	//private:
	//	NodeType* Data = nullptr;
	//	HasherType Hasher = HasherType();
	//	PredicateType Predicate = PredicateType();
	//	AllocatorType Allocator = AllocatorType();
	//
	//	NODISCARD FORCEINLINE NodeType* Allocate(const size_t Count)
	//	{
	//		return ReinterpretCast<NodeType*>(Allocator.Allocate(Count));
	//	}
	//
	//	FORCEINLINE void Deallocate(void* const Ptr, const size_t Count)
	//	{
	//		Allocator.Deallocate(Ptr, sizeof(NodeType) * Count);
	//	}
	//
	//	NODISCARD FORCEINLINE uint32_t GetNodeHeight(const NodeType* const Node) const noexcept
	//	{
	//		return Node ? Node->Height : 0;
	//	}
	//
	//	NODISCARD FORCEINLINE uint32_t GetBalanceFactor(const NodeType* const Node) const noexcept
	//	{
	//		return Node ? GetNodeHeight(Node->Left) - GetNodeHeight(Node->Right) : 0;
	//	}
	//
	//	NODISCARD FORCEINLINE void UpdateNodeHeight(NodeType* const Node) noexcept
	//	{
	//		Node->Height = 1 + std::max(GetNodeHeight(Node->Left), GetNodeHeight(Node->Right));
	//	}
	//
	//	NODISCARD FORCEINLINE NodeType* RotateRight(NodeType* const Node) noexcept
	//	{
	//		NodeType* const Left = Node->Left;
	//		NodeType* const Right = Left->Right;
	//
	//		Left->Right = Node;
	//		Node->Left = Right;
	//
	//		UpdateNodeHeight(Node);
	//		UpdateNodeHeight(Left);
	//
	//		return Left;
	//	}
	//
	//	NODISCARD FORCEINLINE NodeType* RotateLeft(NodeType* const Node) noexcept
	//	{
	//		NodeType* const Right = Node->Right;
	//		NodeType* const Left = Right->Left;
	//
	//		Right->Left = Node;
	//		Node->Right = Left;
	//
	//		UpdateNodeHeight(Node);
	//		UpdateNodeHeight(Right);
	//
	//		return Right;
	//	}
	//};
}