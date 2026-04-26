#pragma once

#include "Diagnostic.h"

#include <string>
#include <vector>

namespace Assembler {

enum class ParsedOperandKind {
    Identifier,
    Number
};

struct ParsedOperand {
    ParsedOperandKind Kind{};
    std::string Text;
    SourceRange Range;
};

struct ParsedInstruction {
    std::string Mnemonic;
	std::vector<ParsedOperand> Operands;
    SourceRange Range;
};

}
