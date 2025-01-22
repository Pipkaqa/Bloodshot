#pragma once

#include <Core.h>

#include <type_traits>

namespace Bloodshot::Test
{
	struct IAssert abstract final
	{
	public:
		template<typename FunctionType, typename... ArgTypes>
		static auto ExpectSuccess(FunctionType&& Function, ArgTypes&&... Args)
		{
			LastErrorCount = FLogger::GetErrorCount();

			if constexpr (std::is_void_v<decltype(Function(std::declval<ArgTypes>()...))>)
			{
				Function(std::forward<ArgTypes>(Args)...);
				BS_LOG_IF(LastErrorCount != FLogger::GetErrorCount(), Fatal, "Success Test Failed");
				return;
			}
			else
			{
				auto ReturnValue = Function(std::forward<ArgTypes>(Args)...);
				BS_LOG_IF(LastErrorCount != FLogger::GetErrorCount(), Fatal, "Success Test Failed");
				return ReturnValue;
			}
		}

		template<typename FunctionType, typename... ArgTypes>
		static auto ExpectError(FunctionType&& Function, ArgTypes&&... Args)
		{
			LastErrorCount = FLogger::GetErrorCount();

			if constexpr (std::is_void_v<decltype(Function(std::declval<ArgTypes>()...))>)
			{
				Function(std::forward<ArgTypes>(Args)...);
				BS_LOG_IF(LastErrorCount == FLogger::GetErrorCount(), Fatal, "Success Test Failed");
				return;
			}
			else
			{
				auto ReturnValue = Function(std::forward<ArgTypes>(Args)...);
				BS_LOG_IF(LastErrorCount == FLogger::GetErrorCount(), Fatal, "Success Test Failed");
				return ReturnValue;
			}
		}

		template<typename T>
		static void AreEqual(const T& First, const T& Second)
		{
			BS_LOG_IF(First != Second, Fatal, "AreEqual Test Failed");
		}

		template<typename T, typename U>
		static void AreEqual(const T& First, const U& Second)
		{
			BS_LOG_IF(First != Second, Fatal, "AreEqual Test Failed");
		}

		template<typename T>
			requires std::is_pointer_v<T>
		static void AreEqual(std::nullptr_t First, const T& Second)
		{
			BS_LOG_IF(First != Second, Fatal, "AreEqual Test Failed");
		}

		template<typename T>
		static void AreNotEqual(const T& First, const T& Second)
		{
			BS_LOG_IF(First == Second, Fatal, "AreEqual Test Failed");
		}

		template<typename T, typename U>
		static void AreNotEqual(const T& First, const U& Second)
		{
			BS_LOG_IF(First == Second, Fatal, "AreEqual Test Failed");
		}

		template<typename T>
			requires std::is_pointer_v<T>
		static void AreNotEqual(std::nullptr_t First, const T& Second)
		{
			BS_LOG_IF(First == Second, Fatal, "AreEqual Test Failed");
		}

	private:
		static inline size_t LastErrorCount = 0;
	};
}
