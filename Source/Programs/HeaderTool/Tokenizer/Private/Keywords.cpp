#include "Keywords.h"

#include <array>

namespace
{
	static inline constexpr std::array<std::string_view, 100> GKeywordArr =
	{
		"alignas",
		"alignof",
		"and",
		"and_eq",
		"asm",
		"auto",
		"bitand",
		"bitor",
		"bool",
		"break",
		"case",
		"catch",
		"char",
		"char16_t",
		"char32_t",
		"class",
		"compl",
		"const",
		"constexpr",
		"const_cast",
		"continue",
		"decltype",
		"default",
		"delete",
		"do",
		"double",
		"dynamic_cast",
		"else",
		"enum",
		"explicit",
		"export",
		"extern",
		"false",
		"float",
		"for",
		"friend",
		"goto",
		"if",
		"inline",
		"int",
		"long",
		"mutable",
		"namespace",
		"new",
		"noexcept",
		"not",
		"not_eq",
		"nullptr",
		"operator",
		"or",
		"or_eq",
		"private",
		"protected",
		"public",
		"register",
		"reinterpret_cast",
		"return",
		"short",
		"signed",
		"sizeof",
		"static",
		"static_assert",
		"static_cast",
		"struct",
		"switch",
		"template",
		"this",
		"thread_local",
		"throw",
		"true",
		"try",
		"typedef",
		"typeid",
		"typename",
		"union",
		"unsigned",
		"using",
		"virtual",
		"void",
		"volatile",
		"wchar_t",
		"while",
		"xor",
		"xor_eq",
		"override",
		"final",
		"concept",
		"requires",
		"co_await",
		"co_return",
		"co_yield",
		"import",
		"module",
		"synchronized",
		"transaction_safe",
		"transaction_safe_dynamic",
		"nothrow",
		"axiom",
		"reflexpr",
		"std"
	};

	static inline constexpr std::array<std::string_view, 11> GOperators =
	{
		"+", "-", "*", "/", "%", "=", "&", "|", "<", ">", "!"
	};

	static inline constexpr std::array<std::string_view, 8> GSeparators =
	{
		"(", ")", "{", "}", "[", "]", ",", ";"
	};
}

namespace Bloodshot::HeaderTool
{
	bool IsKeyword(std::string_view Word)
	{
		return std::find(GKeywordArr.begin(), GKeywordArr.end(), Word) != GKeywordArr.end() ? true : false;
	}

	bool IsIdentifier(std::string_view Word)
	{
		return !IsKeyword(Word);
	}

	bool IsOperator(const char Char)
	{
		return std::find(GOperators.begin(), GOperators.end(), &Char) != GOperators.end();
	}

	bool IsSeparator(const char Char)
	{
		return std::find(GSeparators.begin(), GSeparators.end(), &Char) != GSeparators.end();
	}
}
