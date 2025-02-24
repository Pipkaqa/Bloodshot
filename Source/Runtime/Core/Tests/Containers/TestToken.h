#pragma once

#include "Misc/AssertionMacros.h"

#include <cstdint>

namespace Bloodshot::Test
{
	template<typename InElementType>
	class TTestToken
	{
	public:
		TTestToken() noexcept
		{
			ConstructorCalls++;
		}

		TTestToken(const TTestToken& Other)
			: Value(Other.Value)
		{
			BS_CHECK(&Other != this);
			CopyConstructorCalls++;
		}

		TTestToken(TTestToken&& Other) noexcept
			: Value(std::move(Other.Value))
		{
			BS_CHECK(&Other != this);
			++MoveConstructorCalls;
		}

		TTestToken(const InElementType& InValue) noexcept
		{
			ConstructorCalls++;
			Value = InValue;
		}

		~TTestToken()
		{
			DestructorCalls++;
		}

		uint32_t& operator*()
		{
			return Value;
		}

		const uint32_t& operator*() const
		{
			return Value;
		}

		uint32_t* operator->()
		{
			return &Value;
		}

		const uint32_t* operator->() const
		{
			return &Value;
		}

		TTestToken& operator=(const TTestToken& Other)
		{
			BS_CHECK(&Other != this);
			Value = Other.Value;
			CopyAssignmentCalls++;
			return *this;
		}

		TTestToken& operator=(TTestToken&& Other) noexcept
		{
			BS_CHECK(&Other != this);
			Value = std::move(Other.Value);
			MoveAssignmentCalls++;
			return *this;
		}

		bool operator<(const TTestToken& Other) const
		{
			return Value < Other.Value;
		}

		static uint32_t GetConstructionCalls()
		{
			return ConstructorCalls + CopyConstructorCalls + MoveConstructorCalls;
		}

		static uint32_t GetConstructorCalls()
		{
			return ConstructorCalls;
		}

		static uint32_t GetCopyConstructorCalls()
		{
			return CopyConstructorCalls;
		}

		static uint32_t GetMoveConstructorCalls()
		{
			return MoveConstructorCalls;
		}

		static uint32_t GetConstructionDestructionCallDifference()
		{
			return GetConstructionCalls() - DestructorCalls;
		}

		static uint32_t GetCopyCalls()
		{
			return CopyConstructorCalls + CopyAssignmentCalls;
		}

		static uint32_t GetCopyAssignmentCalls()
		{
			return CopyAssignmentCalls;
		}

		static uint32_t GetMoveCalls()
		{
			return MoveConstructorCalls + MoveAssignmentCalls;
		}

		static uint32_t GetMoveAssignmentCalls()
		{
			return MoveAssignmentCalls;
		}

		static uint32_t GetDestructionCalls()
		{
			return DestructorCalls;
		}

		static bool EvenConstructionDestructionCalls()
		{
			return GetConstructionDestructionCallDifference() == 0;
		}

		static bool EvenConstructionDestructionCalls(const uint32_t Expected)
		{
			return EvenConstructionDestructionCalls() && DestructorCalls == Expected;
		}

		static void Reset()
		{
			ConstructorCalls = 0;
			DestructorCalls = 0;
			CopyConstructorCalls = 0;
			MoveConstructorCalls = 0;
			CopyAssignmentCalls = 0;
			MoveAssignmentCalls = 0;
		}

		InElementType Value = InElementType();

	private:
		static inline uint32_t ConstructorCalls;
		static inline uint32_t DestructorCalls;
		static inline uint32_t CopyConstructorCalls;
		static inline uint32_t MoveConstructorCalls;
		static inline uint32_t CopyAssignmentCalls;
		static inline uint32_t MoveAssignmentCalls;
	};

	using FTestToken = TTestToken<uint64_t>;
}
