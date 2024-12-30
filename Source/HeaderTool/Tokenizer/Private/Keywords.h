#pragma once

#include <algorithm>
#include <array>
#include <string>

namespace Bloodshot::HeaderTool::Private
{
	inline const std::array<std::string, 100> GKeywordArr =
	{
		"alignas", "alignof",
		"and", "and_eq",
		"asm", "auto",
		"bitand", "bitor",
		"bool", "break",
		"case", "catch",
		"char", "char16_t",
		"char32_t", "class",
		"compl", "const",
		"constexpr", "const_cast",
		"continue", "decltype",
		"default", "delete",
		"do", "double",
		"dynamic_cast", "else",
		"enum", "explicit",
		"export", "extern",
		"false", "float",
		"for", "friend",
		"goto", "if",
		"inline", "int",
		"long", "mutable",
		"namespace", "new",
		"noexcept", "not",
		"not_eq", "nullptr",
		"operator", "or",
		"or_eq", "private",
		"protected", "public",
		"register", "reinterpret_cast",
		"return", "short",
		"signed", "sizeof",
		"static", "static_assert",
		"static_cast", "struct",
		"switch", "template",
		"this", "thread_local",
		"throw", "true",
		"try", "typedef",
		"typeid", "typename",
		"union", "unsigned",
		"using", "virtual",
		"void", "volatile",
		"wchar_t", "while",
		"xor", "xor_eq",
		"override", "final",
		"concept", "requires",
		"co_await", "co_return",
		"co_yield", "import",
		"module", "synchronized",
		"transaction_safe", "transaction_safe_dynamic",
		"nothrow", "axiom",
		"reflexpr", "std"
	};

	inline const std::string& GOperators = "+-*/%=&|<>!";
	inline const std::string& GSeparators = "(){}[],;";

	bool IsOperator(std::string_view Char);
	bool IsSeparator(std::string_view Char);
	bool IsOperator(const char Char);
	bool IsSeparator(const char Char);
	bool IsKeyword(std::string_view Word);
}