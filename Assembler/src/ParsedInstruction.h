#pragma once

#include "Diagnostic.h"

#include <string>
#include <vector>

namespace Assembler {
enum class ParsedOperandKind : uint8_t
{
	Identifier,
	Number
};

struct ParsedOperand
{
	ParsedOperandKind Kind{};
	std::string Text;
	SourceRange Range;
};

struct ParsedInstruction
{
	std::string Mnemonic;
	std::vector<ParsedOperand> Operands;
	SourceRange Range;
};

enum class StatementKind : uint8_t
{
	Instruction,
	Label
};

struct Statement
{
	StatementKind Kind{};
	ParsedInstruction Instruction;
	std::string LabelName;
	SourceRange Range;
};
}