#pragma once

#include "DiagnosticBag.h"
#include "ParsedInstruction.h"
#include "Token.h"

#include <span>
#include <vector>

namespace Assembler {
class Parser
{
public:
	explicit Parser(std::span<const Token> tokens);

	std::vector<Statement> Parse(DiagnosticBag& diagnostics);

private:
	std::span<const Token> m_Tokens;
	size_t m_Position = 0;

	const Token& Current() const
	{
		return m_Tokens[m_Position];
	}

	const Token& Advance()
	{
		return m_Tokens[m_Position++];
	}

	bool IsAtEnd() const
	{
		return m_Position >= m_Tokens.size() || Current().Kind == TokenKind::EndOfFile;
	}

	void SkipLine()
	{
		while (!IsAtEnd() && Current().Kind != TokenKind::EndOfLine) {
			Advance();
		}
		if (!IsAtEnd() && Current().Kind == TokenKind::EndOfLine) {
			Advance();
		}
	}

	void SkipEmptyLines()
	{
		while (!IsAtEnd() && Current().Kind == TokenKind::EndOfLine) {
			Advance();
		}
	}
};
}