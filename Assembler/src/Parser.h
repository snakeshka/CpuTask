#pragma once

#include "DiagnosticBag.h"
#include "ParsedInstruction.h"
#include "Token.h"

#include <span>
#include <vector>

namespace Assembler {

class Parser {
public:
    explicit Parser(std::span<const Token> tokens);

    std::vector<ParsedInstruction> Parse(DiagnosticBag &diagnostics);

private:
    std::span<const Token> m_Tokens;
    size_t m_Position = 0;

    const Token& Current() const;
    const Token& Advance();
    bool IsAtEnd() const;
    void SkipLine();
    void SkipEmptyLines();
};

}
