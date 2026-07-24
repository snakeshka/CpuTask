#pragma once

#include "Diagnostic.h"

#include <string>

namespace Assembler {
enum class TokenKind
{
	Identifier,
	Number,
	Colon,
	EndOfLine,
	EndOfFile,
	Unknown
};

struct Token
{
	TokenKind Kind{};
	std::string Text;
	SourceRange Range;
};
}