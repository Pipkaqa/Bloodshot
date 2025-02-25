#pragma once

#include "Allocators/Allocator.h"
#include "Misc/AssertionMacros.h"
#include "Misc/Casts.h"
#include "Platform/Platform.h"

namespace Bloodshot
{
	namespace Private::Containers::Set
	{
		template<typename T>
		class TSetNode
		{
		public:
			T Data;
			TSetNode* Left;
			TSetNode* Right;
			int16_t Height;
		};
	}

	template<typename InElementType,
		typename InPredicateType = std::less<InElementType>,
		IsAllocator InAllocatorType = TAllocator<InElementType>>
	class TSet
	{
	public:
		using ElementType = InElementType;
		using AllocatorType = InAllocatorType;

	private:
		using NodeType = Private::Containers::Set::TSetNode<ElementType>;
		using NodeAllocatorType = AllocatorType::template Rebind<NodeType>;

	public:
		using PredicateType = InPredicateType;

		NODISCARD FORCEINLINE NodeAllocatorType& GetAllocator() noexcept
		{
			return Allocator;
		}

		NODISCARD FORCEINLINE const NodeAllocatorType& GetAllocator() const noexcept
		{
			return Allocator;
		}

		NODISCARD FORCEINLINE NodeType* GetData() noexcept
		{
			return Root;
		}

		NODISCARD FORCEINLINE const NodeType* GetData() const noexcept
		{
			return Root;
		}

		NODISCARD FORCEINLINE size_t GetSize() const noexcept
		{
			return Size;
		}

		NODISCARD FORCEINLINE bool IsEmpty() const noexcept
		{
			return !Size;
		}

		FORCEINLINE ElementType* Add(const ElementType& InElement)
		{
			NodeType* NewNode = nullptr;
			Root = SubTreeAddUninitialized(Root, NewNode, InElement);
			if (NewNode)
			{
				ElementType* const NewElement = &NewNode->Data;
				new(NewElement) ElementType(InElement);
				return NewElement;
			}
			return nullptr;
		}

		FORCEINLINE ElementType* Add(ElementType&& InElement) noexcept
		{
			NodeType* NewNode = nullptr;
			Root = SubTreeAddUninitialized(Root, NewNode, InElement);
			if (NewNode)
			{
				ElementType* const NewElement = &NewNode->Data;
				new(NewElement) ElementType(std::move(InElement));
				return NewElement;
			}
			return nullptr;
		}

		FORCEINLINE bool Remove(const ElementType& InElement) noexcept
		{
			bool Removed = false;
			Root = SubTreeRemove(Root, Removed, InElement);
			return Removed;
		}

		FORCEINLINE void Clear() noexcept
		{
			SubTreeClear(Root);
			Root = nullptr;
			Size = 0;
		}

		NODISCARD FORCEINLINE const ElementType* FindMin() const noexcept
		{
			if (!Root)
			{
				return nullptr;
			}
			return &SubTreeMin(Root)->Data;
		}

		NODISCARD FORCEINLINE const ElementType* FindMax() const noexcept
		{
			if (!Root)
			{
				return nullptr;
			}
			return &SubTreeMax(Root)->Data;
		}

		NODISCARD FORCEINLINE bool Contains(const ElementType& InElement) const noexcept
		{
			return SubTreeFind(Root, InElement);
		}

		NODISCARD FORCEINLINE const ElementType* Find(const ElementType& InElement) const noexcept
		{
			const NodeType* const Result = SubTreeFind(Root, InElement);
			if (Result)
			{
				return &Result->Data;
			}
			return nullptr;
		}

	private:
		PredicateType Predicate = PredicateType();
		NodeAllocatorType Allocator = NodeAllocatorType();

		NodeType* Root = nullptr;
		size_t Size = 0;

		NODISCARD FORCEINLINE int16_t GetHeight(const NodeType* const InNode) const noexcept
		{
			return InNode ? InNode->Height : 0;
		}

		NODISCARD FORCEINLINE int16_t GetBalanceFactor(const NodeType* const InNode) const noexcept
		{
			return InNode ? GetHeight(InNode->Right) - GetHeight(InNode->Left) : 0;
		}

		NODISCARD void UpdateHeight(NodeType* const InNode) noexcept
		{
			InNode->Height = 1 + std::max(GetHeight(InNode->Left), GetHeight(InNode->Right));
		}

		NODISCARD FORCEINLINE NodeType* RotateLeft(NodeType* const InNode) noexcept
		{
			NodeType* const Right = InNode->Right;
			NodeType* const Left = Right->Left;

			Right->Left = InNode;
			InNode->Right = Left;

			UpdateHeight(InNode);
			UpdateHeight(Right);

			return Right;
		}

		NODISCARD FORCEINLINE NodeType* RotateRight(NodeType* const InNode) noexcept
		{
			NodeType* const Left = InNode->Left;
			NodeType* const Right = Left->Right;

			Left->Right = InNode;
			InNode->Left = Right;

			UpdateHeight(InNode);
			UpdateHeight(Left);

			return Left;
		}

		NODISCARD FORCEINLINE NodeType* Balance(NodeType* const InNode) noexcept
		{
			if (!InNode) return InNode;

			UpdateHeight(InNode);
			const int16_t BalanceFactor = GetBalanceFactor(InNode);
			if (BalanceFactor < -1)
			{
				if (GetBalanceFactor(InNode->Left) > 0)
				{
					InNode->Left = RotateLeft(InNode->Left);
				}
				return RotateRight(InNode);
			}
			if (BalanceFactor > 1)
			{
				if (GetBalanceFactor(InNode->Right) < 0)
				{
					InNode->Right = RotateRight(InNode->Right);
				}
				return RotateLeft(InNode);
			}

			return InNode;
		}

		NODISCARD NodeType* SubTreeAddUninitialized(NodeType* const InNode, NodeType*& OutNewNode, const ElementType& InElement) noexcept
		{
			if (!InNode)
			{
				NodeType* const NewNode = Allocate();
				NewNode->Left = nullptr;
				NewNode->Right = nullptr;
				NewNode->Height = 1;
				OutNewNode = NewNode;
				++Size;
				return NewNode;
			}

			if (Predicate(InElement, InNode->Data))
			{
				InNode->Left = SubTreeAddUninitialized(InNode->Left, OutNewNode, InElement);
			}
			else if (Predicate(InNode->Data, InElement))
			{
				InNode->Right = SubTreeAddUninitialized(InNode->Right, OutNewNode, InElement);
			}
			else
			{
				return InNode;
			}

			return Balance(InNode);
		}

		NODISCARD NodeType* SubTreeRemove(NodeType* const InNode, bool& OutRemoved, const ElementType& InElement) noexcept
		{
			if (!InNode) return InNode;

			if (Predicate(InElement, InNode->Data))
			{
				InNode->Left = SubTreeRemove(InNode->Left, OutRemoved, InElement);
			}
			else if (Predicate(InNode->Data, InElement))
			{
				InNode->Right = SubTreeRemove(InNode->Right, OutRemoved, InElement);
			}
			else
			{
				if (OutRemoved)
				{
					return nullptr;
				}

				ElementType* const Dest = &InNode->Data;
				DestructElement(Dest);

				const bool HasLeft = InNode->Left;
				const bool HasRight = InNode->Right;

				if (HasLeft && HasRight)
				{
					NodeType* const Successor = SubTreeMin(Root);
					MoveConstructElements(Dest, &Successor->Data, 1);
					--Size;
					InNode->Right = SubTreeRemove(InNode->Right, OutRemoved, *Dest);
				}
				else if (!HasLeft && !HasRight)
				{
					Deallocate(InNode);
					OutRemoved = true;
					return nullptr;
				}
				else
				{
					NodeType* const Child = std::exchange(HasLeft ? InNode->Left : InNode->Right, nullptr);
					MoveConstructElements(Dest, &Child->Data, 1);
					Deallocate(Child);
					OutRemoved = true;
				}
			}

			return Balance(InNode);
		};

		void SubTreeClear(NodeType* const InNode) noexcept
		{
			if (InNode)
			{
				SubTreeClear(InNode->Left);
				SubTreeClear(InNode->Right);
				DestructElement(&InNode->Data);
				Deallocate(InNode);
			}
		}

		NODISCARD FORCEINLINE NodeType* SubTreeFind(NodeType* const InNode, const ElementType& InElement) const noexcept
		{
			NodeType* Current = InNode;
			while (true)
			{
				if (Current)
				{
					if (Predicate(InElement, Current->Data))
					{
						Current = Current->Left;
					}
					else if (Predicate(Current->Data, InElement))
					{
						Current = Current->Right;
					}
					else
					{
						return Current;
					}
				}
				else
				{
					return nullptr;
				}
			}
		}

		NODISCARD FORCEINLINE NodeType* SubTreeMin(NodeType* const InNode) const noexcept
		{
			BS_CHECK(InNode);
			NodeType* Current = InNode;
			while (Current && Current->Left)
			{
				Current = Current->Left;
			}
			return Current;
		}

		NODISCARD FORCEINLINE NodeType* SubTreeMax(NodeType* const InNode) const noexcept
		{
			BS_CHECK(InNode);
			NodeType* Current = InNode;
			while (Current && Current->Right)
			{
				Current = Current->Right;
			}
			return Current;
		}

		NODISCARD FORCEINLINE NodeType* Allocate() noexcept
		{
			return ReinterpretCast<NodeType*>(Allocator.Allocate(1));
		}

		FORCEINLINE void Deallocate(void* const Ptr) noexcept
		{
			Allocator.Deallocate(Ptr, sizeof(NodeType) * 1);
		}
	};
}
