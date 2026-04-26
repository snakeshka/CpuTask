#include "Parser.h"

namespace Assembler {
namespace {

bool isOperandToken(TokenKind kind)
{
    return kind == TokenKind::Identifier || kind == TokenKind::Number;
}

ParsedOperandKind toParsedOperandKind(TokenKind kind)
{
    return kind == TokenKind::Number ? ParsedOperandKind::Number : ParsedOperandKind::Identifier;
}

}

Parser::Parser(std::span<const Token> tokens)
    : m_Tokens(tokens)
{
}

std::vector<ParsedInstruction> Parser::Parse(DiagnosticBag& diagnostics)
{
    std::vector<ParsedInstruction> instructions;

    while (!IsAtEnd()) {
        SkipEmptyLines();
        if (IsAtEnd()) {
            break;
        }

        if (Current().Kind != TokenKind::Identifier) {
            diagnostics.Add(Diagnostic{
                DiagnosticSeverity::Error,
                DiagnosticCode::UnexpectedToken,
                Current().Range,
                "expected instruction mnemonic",
            });
            SkipLine();
            continue;
        }

        const auto mnemonic = Advance();
        ParsedInstruction instruction;
        instruction.Mnemonic = mnemonic.Text;
        instruction.Range = mnemonic.Range;

        while (!IsAtEnd() && Current().Kind != TokenKind::EndOfLine && Current().Kind != TokenKind::EndOfFile) {
            if (!isOperandToken(Current().Kind)) {
                diagnostics.Add(Diagnostic{
                    DiagnosticSeverity::Error,
                    DiagnosticCode::UnexpectedToken,
                    Current().Range,
                    "unexpected token \"" + Current().Text + "\"",
                });
                SkipLine();
                break;
            }

            const auto operand = Advance();
            instruction.Range.End = operand.Range.End;
            instruction.Operands.push_back(ParsedOperand{toParsedOperandKind(operand.Kind), operand.Text, operand.Range});
        }

        instructions.push_back(std::move(instruction));
        if (!IsAtEnd() && Current().Kind == TokenKind::EndOfLine) {
            Advance();
        }
    }

    return instructions;
}

const Token& Parser::Current() const
{
    return m_Tokens[m_Position];
}

const Token& Parser::Advance()
{
    return m_Tokens[m_Position++];
}

bool Parser::IsAtEnd() const
{
    return m_Position >= m_Tokens.size() || Current().Kind == TokenKind::EndOfFile;
}

void Parser::SkipLine()
{
    while (!IsAtEnd() && Current().Kind != TokenKind::EndOfLine) {
        Advance();
    }
    if (!IsAtEnd() && Current().Kind == TokenKind::EndOfLine) {
        Advance();
    }
}

void Parser::SkipEmptyLines()
{
    while (!IsAtEnd() && Current().Kind == TokenKind::EndOfLine) {
        Advance();
    }
}

}
