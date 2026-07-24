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

std::vector<Statement> Parser::Parse(DiagnosticBag& diagnostics)
{
	std::vector<Statement> statements;

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

		// Проверяем, является ли это объявлением метки (Identifier ':')
		if (m_Position + 1 < m_Tokens.size() && m_Tokens[m_Position + 1].Kind == TokenKind::Colon) {
			const auto labelToken = Advance();
			Advance();

			Statement stmt;
			stmt.Kind = StatementKind::Label;
			stmt.LabelName = labelToken.Text;
			stmt.Range = labelToken.Range;
			statements.push_back(std::move(stmt));

			if (!IsAtEnd() && Current().Kind == TokenKind::EndOfLine) {
				Advance();
			}
			continue;
		}

		// Парсинг обычной инструкции
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

		Statement stmt;
		stmt.Kind = StatementKind::Instruction;
		stmt.Instruction = std::move(instruction);
		stmt.Range = stmt.Instruction.Range;
		statements.push_back(std::move(stmt));

		if (!IsAtEnd() && Current().Kind == TokenKind::EndOfLine) {
			Advance();
		}
	}

	return statements;
}
}