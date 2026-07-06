#pragma once

#include "DiagnosticBag.h"
#include "Token.h"

#include <string_view>
#include <vector>

namespace Assembler {

class Lexer {
public:
    explicit Lexer(std::string_view source);

    std::vector<Token> Lex(DiagnosticBag& diagnostics);

private:
    std::string_view m_Source;
    size_t m_Position = 0;
    SourceLocation m_Location{};

    bool IsAtEnd() const;
    char Peek() const;
    char Advance();
    SourceRange MakeRange(SourceLocation begin) const;
    void LexIdentifier(std::vector<Token>& tokens);
    void LexNumber(std::vector<Token>& tokens, DiagnosticBag& diagnostics);
};

}
