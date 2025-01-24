#pragma once

#include "Object/Class.h"
#include "Object/Object.h"
#include "Reflection/Mirror.h"
#include "Templates/Containers/Array.h"
#include "Templates/Containers/List.h"
#include "Templates/Containers/Map.h"
#include "Templates/Containers/Pair.h"
#include "Templates/Containers/Set.h"
#include "Templates/Containers/Tuple.h"
#include "Templates/Containers/Vector.h"
#include "Templates/SmartPointers.h"

#include <filesystem>
#include <format>
#include <fstream>

namespace Bloodshot
{
	template <typename T>
	concept IsContainer = requires(T t)
	{
		typename T::iterator;
		typename T::value_type;
		{ t.begin() } -> std::same_as<typename T::iterator>;
		{ t.end() } -> std::same_as<typename T::iterator>;
	};

	class FSerializer final
	{
	public:
		FSerializer(const std::filesystem::path& OutputPath);

		template<IsObject T>
		void ToYAML(const T& Object);

		template<typename T>
		void ToYAML(const T& Object, FStringView Name)
		{
			WriteLine("{}: {}", Name, Object);
		}

		template<IsContainer T>
		void ToYAML(const T& Container, FStringView Name)
		{
			WriteLine("{}:", Name);
			PushScope();

			for (const T::value_type& Element : Container)
			{
				ToYAML(Element);
			}

			PopScope();
		}

		template<typename ElementType1, typename ElementType2>
		void ToYAML(const TPair<ElementType1, ElementType2>& Pair, FStringView Name)
		{
			WriteLine("{}: [{}, {}]", Name, Pair.first, Pair.second);
		}

		template<typename ElementType, typename... ElementTypes>
		void ToYAML(const TTuple<ElementType, ElementTypes...>& Tuple, FStringView Name)
		{
			std::string Result = std::format("{}: [", Name);

			std::apply([&Result](auto&&... Args)
				{
					((Result += std::format("{}, ", Args)), ...);
				}, Tuple);

			Result.pop_back();
			Result.pop_back();
			Result += "]";

			WriteLine(Result);
		}

	private:
		std::ofstream OutputStream;
		size_t PushedScopes = 0;

		void WriteLine(std::string_view String);

		template<typename... ArgTypes>
		void WriteLine(const std::format_string<ArgTypes...>& Format, ArgTypes&&... Args)
		{
			for (size_t i = 0; i < PushedScopes; ++i)
			{
				OutputStream << "\t";
			}

			OutputStream << std::format(Format, std::forward<ArgTypes>(Args)...) << "\n";
		}

		void EmptyLine();

		void PushScope();
		void PopScope();

		template<typename T>
		void ToYAML(const T& Object)
		{
			WriteLine("Value: {}", Object);
		}

		template<IsContainer T>
		void ToYAML(const T& Container)
		{
			WriteLine("Value:");
			PushScope();

			for (const T::value_type& Element : Container)
			{
				ToYAML(Element);
			}

			PopScope();
		}

		template<typename ElementType1, typename ElementType2>
		void ToYAML(const TPair<ElementType1, ElementType2>& Pair)
		{
			WriteLine("Value: [{}, {}]", Pair.first, Pair.second);
		}

		template<typename ElementType, typename... ElementTypes>
		void ToYAML(const TTuple<ElementType, ElementTypes...>& Tuple)
		{
			std::string Result = "Value: [";

			std::apply([&Result](auto&&... Args)
				{
					((Result += std::format("{}, ", Args)), ...);
				}, Tuple);

			Result.pop_back();
			Result.pop_back();
			Result += "]";

			WriteLine(Result);
		}
	};
}
