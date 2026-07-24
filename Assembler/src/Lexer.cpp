#include "Lexer.h"

#include <cctype>

namespace Assembler {
namespace {
	bool IsIdentifierStart(char ch)
	{
		return std::isalpha(static_cast<unsigned char>(ch)) != 0 || ch == '_';
	}

	bool IsIdentifierPart(char ch)
	{
		return std::isalnum(static_cast<unsigned char>(ch)) != 0 || ch == '_';
	}

	bool IsHexDigit(char ch)
	{
		return std::isxdigit(static_cast<unsigned char>(ch)) != 0;
	}
}

Lexer::Lexer(std::string_view source)
	: m_Source(source)
{
}

std::vector<Token> Lexer::Lex(DiagnosticBag& diagnostics)
{
	std::vector<Token> tokens;

	while (!IsAtEnd()) {
		const char ch = Peek();
		if (ch == ' ' || ch == '\t' || ch == '\r') {
			Advance();
			continue;
		}

		if (ch == ';') {
			while (!IsAtEnd() && Peek() != '\n') {
				Advance();
			}
			continue;
		}

		if (ch == ':') {
			const auto begin = m_Location;
			Advance();
			tokens.push_back(Token{TokenKind::Colon, ":", MakeRange(begin)});
			continue;
		}

		if (ch == '\n') {
			const auto begin = m_Location;
			Advance();
			tokens.push_back(Token{TokenKind::EndOfLine, "", MakeRange(begin)});
			continue;
		}

		if (ch == '-' && m_Position + 1 < m_Source.size() && std::isdigit(static_cast<unsigned char>(m_Source[m_Position + 1])) != 0) {
			LexNumber(tokens, diagnostics);
			continue;
		}

		if (IsIdentifierStart(ch)) {
			LexIdentifier(tokens);
			continue;
		}

		if (std::isdigit(static_cast<unsigned char>(ch)) != 0) {
			LexNumber(tokens, diagnostics);
			continue;
		}

		const auto begin = m_Location;
		const auto text = std::string(1, Advance());
		tokens.push_back(Token{TokenKind::Unknown, text, MakeRange(begin)});
		diagnostics.Add(Diagnostic{
			DiagnosticSeverity::Error,
			DiagnosticCode::UnknownCharacter,
			tokens.back().Range,
			"unknown character \"" + text + "\"",
		});
	}

	tokens.push_back(Token{TokenKind::EndOfFile, "", SourceRange{m_Location, m_Location}});
	return tokens;
}

bool Lexer::IsAtEnd() const
{
	return m_Position >= m_Source.size();
}

char Lexer::Peek() const
{
	return m_Source[m_Position];
}

char Lexer::Advance()
{
	const char ch = m_Source[m_Position++];
	if (ch == '\n') {
		++m_Location.Line;
		m_Location.Column = 1;
	} else {
		++m_Location.Column;
	}

	return ch;
}

SourceRange Lexer::MakeRange(SourceLocation begin) const
{
	return SourceRange{begin, m_Location};
}

void Lexer::LexIdentifier(std::vector<Token>& tokens)
{
	const auto beginLocation = m_Location;
	const auto begin = m_Position;
	while (!IsAtEnd() && IsIdentifierPart(Peek())) {
		Advance();
	}

	tokens.push_back(Token{TokenKind::Identifier, std::string(m_Source.substr(begin, m_Position - begin)), MakeRange(beginLocation)});
}

void Lexer::LexNumber(std::vector<Token>& tokens, DiagnosticBag& diagnostics)
{
	const auto beginLocation = m_Location;
	const auto begin = m_Position;
	bool valid = true;

	if (Peek() == '-') {
		Advance();
	}

	if (Peek() == '0' && m_Position + 1 < m_Source.size() && (m_Source[m_Position + 1] == 'x' || m_Source[m_Position + 1] == 'X')) {
		Advance();
		Advance();
		const auto digitBegin = m_Position;
		while (!IsAtEnd() && std::isalnum(static_cast<unsigned char>(Peek())) != 0) {
			if (!IsHexDigit(Peek())) {
				valid = false;
			}
			Advance();
		}
		valid = valid && m_Position > digitBegin;
	} else {
		while (!IsAtEnd() && std::isalnum(static_cast<unsigned char>(Peek())) != 0) {
			if (std::isdigit(static_cast<unsigned char>(Peek())) == 0) {
				valid = false;
			}
			Advance();
		}
	}

	const auto text = std::string(m_Source.substr(begin, m_Position - begin));
	const auto range = MakeRange(beginLocation);
	tokens.push_back(Token{TokenKind::Number, text, range});

	if (!valid) {
		diagnostics.Add(Diagnostic{
			DiagnosticSeverity::Error,
			DiagnosticCode::InvalidNumberLiteral,
			range,
			"invalid number literal \"" + text + "\"",
		});
	}
}
}