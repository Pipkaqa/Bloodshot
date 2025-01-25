#pragma once

#include "Object/Class.h"
#include "Object/Object.h"
#include "Reflection/Mirror.h"
#include "Templates/Containers/Array.h"
#include "Templates/Containers/List.h"
#include "Templates/Containers/Map.h"
#include "Templates/Containers/Pair.h"
#include "Templates/Containers/Set.h"
#include "Templates/Containers/String.h"
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

	class FArchive final
	{
	public:
		FArchive(const std::filesystem::path& OutputPath);
		~FArchive();

		void Serialize(const FString& Object, FStringView Name)
		{
			WriteMode = EWriteMode::Shift;
			Write("{}: {}", Name, Object);
		}

		void Serialize(FStringView Object, FStringView Name)
		{
			WriteMode = EWriteMode::Shift;
			Write("{}: {}", Name, Object);
		}

		template<IsObject T>
		void Serialize(const T& Object);

		template<typename T>
		void Serialize(const T& Object, FStringView Name)
		{
			WriteMode = EWriteMode::Shift;
			Write("{}: {}", Name, Object);
		}

		template<IsContainer T>
		void Serialize(const T& Container, FStringView Name)
		{
			PushWriteState(EWriteState::None);
			WriteMode = EWriteMode::Shift;
			Write("{}:", Name);
			WriteMode = EWriteMode::Append;
			PushScope();

			for (const T::value_type& Element : Container)
			{
				Serialize(Element);
			}

			PopScope();
			PopWriteState();
		}

		template<typename ElementType1, typename ElementType2>
		void Serialize(const TPair<ElementType1, ElementType2>& Pair, FStringView Name)
		{
			PushWriteState(EWriteState::Sequence);
			WriteMode = EWriteMode::Shift;
			Write("{}: [", Name);
			WriteMode = EWriteMode::Append;
			Serialize(Pair.first);
			Write(", ");
			Serialize(Pair.second);
			Write("]");
			PopWriteState();
		}

		template<typename ElementType, typename... ElementTypes>
		void Serialize(const TTuple<ElementType, ElementTypes...>& Tuple, FStringView Name)
		{
			PushWriteState(EWriteState::Sequence);
			WriteMode = EWriteMode::Shift;
			Write("{}: [", Name);
			WriteMode = EWriteMode::Append;

			auto Proxy = [this](const auto& Arg)
				{
					Serialize(Arg);
					Write(", ");;
				};

			std::apply([&Proxy](const auto&... Args)
				{
					(Proxy(Args), ...);
				}, Tuple);

			PopBack();
			PopBack();

			Write("]");
			PopWriteState();
		}

		void Flush();

	private:
		enum class EWriteMode : uint8_t
		{
			Shift = 0,
			Append
		};

		enum class EWriteState : uint8_t
		{
			None = 0,
			Container,
			Sequence
		};

		FString Buffer;
		std::ofstream OutputStream;

		size_t PushedScopes = 0;
		EWriteMode WriteMode = EWriteMode::Append;

		TList<EWriteState> WriteStateStack;

		inline void Write(std::string_view String)
		{
			std::string Str = std::string(String);

			switch (WriteMode)
			{
				case EWriteMode::Shift:
				{
					Buffer += "\n";

					for (size_t i = 0; i < PushedScopes; ++i)
					{
						Buffer += "\t";
					}

					Buffer += Str;
					break;
				}
				case EWriteMode::Append:
				{
					Buffer += Str;
					break;
				}
			}
		}

		template<typename... ArgTypes>
		void Write(const std::format_string<ArgTypes...>& Format, ArgTypes&&... Args)
		{
			std::string Formatted = std::format(Format, std::forward<ArgTypes>(Args)...);

			switch (WriteMode)
			{
				case EWriteMode::Shift:
				{
					Buffer += "\n";

					for (size_t i = 0; i < PushedScopes; ++i)
					{
						Buffer += "\t";
					}

					Buffer += Formatted;
					break;
				}
				case EWriteMode::Append:
				{
					Buffer += Formatted;
					break;
				}
			}
		}

		EWriteState GetWriteState() const;

		void PushScope();
		void PopScope();

		void PushWriteState(const EWriteState WriteState);
		void PopWriteState();

		void PopBack();

		void Serialize(const FString& Object)
		{
			Write("{}", Object);
		}

		void Serialize(FStringView Object)
		{
			Write("{}", Object);
		}

		template<typename T>
		void Serialize(const T& Object)
		{
			Write("{}", Object);
		}

		template<IsContainer T>
		void Serialize(const T& Container)
		{
			if (GetWriteState() == EWriteState::None)
			{
				WriteMode = EWriteMode::Shift;
				Write("{");
				WriteMode = EWriteMode::Append;
			}
			else
			{
				Write("{");
			}

			PushWriteState(EWriteState::Container);

			if (Container.size())
			{
				for (const T::value_type& Element : Container)
				{
					Serialize(Element);
					Write(", ");
				}
				PopBack();
				PopBack();
			}

			PopWriteState();

			Write("}");
		}

		template<typename ElementType1, typename ElementType2>
		void Serialize(const TPair<ElementType1, ElementType2>& Pair)
		{
			if (GetWriteState() == EWriteState::None)
			{
				WriteMode = EWriteMode::Shift;
				Write("[");
				WriteMode = EWriteMode::Append;
			}
			else
			{
				Write("[");
			}

			PushWriteState(EWriteState::Sequence);

			Serialize(Pair.first);
			Write(", ");
			Serialize(Pair.second);

			PopWriteState();

			Write("]");
		}

		template<typename ElementType, typename... ElementTypes>
		void Serialize(const TTuple<ElementType, ElementTypes...>& Tuple)
		{
			if (GetWriteState() == EWriteState::None)
			{
				WriteMode = EWriteMode::Shift;
				Write("[");
				WriteMode = EWriteMode::Append;
			}
			else
			{
				Write("[");
			}

			PushWriteState(EWriteState::Sequence);

			auto Proxy = [this](const auto& Arg)
				{
					Serialize(Arg);
					Write(", ");
				};

			std::apply([&Proxy](const auto&... Args)
				{
					(Proxy(Args), ...);
				}, Tuple);

			PopBack();
			PopBack();

			PopWriteState();

			Write("]");
		}
	};
}
