#include "Lexer.h"

#include <gtest/gtest.h>

namespace
{
bool hasCode(const Assembler::DiagnosticBag &diagnostics, Assembler::DiagnosticCode code)
{
	for (const auto &diagnostic : diagnostics.diagnostics())
	{
		if (diagnostic.Code == code)
		{
			return true;
		}
	}

	return false;
}

}

TEST(LexerTests, LexesLiInstruction)
{
	Assembler::DiagnosticBag diagnostics;
	Assembler::Lexer lexer("LI R0 0x1\n");

	const auto tokens = lexer.Lex(diagnostics);

	ASSERT_FALSE(diagnostics.HasErrors());

	ASSERT_GE(tokens.size(), 5);

	EXPECT_EQ(tokens[0].Kind, Assembler::TokenKind::Identifier);
	EXPECT_EQ(tokens[0].Text, "LI");

	EXPECT_EQ(tokens[1].Kind, Assembler::TokenKind::Identifier);
	EXPECT_EQ(tokens[1].Text, "R0");

	EXPECT_EQ(tokens[2].Kind, Assembler::TokenKind::Number);
	EXPECT_EQ(tokens[2].Text, "0x1");

	EXPECT_EQ(tokens[3].Kind, Assembler::TokenKind::EndOfLine);
	EXPECT_EQ(tokens[4].Kind, Assembler::TokenKind::EndOfFile);
}

TEST(LexerTests, LexesAddInstruction)
{
	Assembler::DiagnosticBag diagnostics;
	Assembler::Lexer lexer("ADD R1 R1 R0\n");

	const auto tokens = lexer.Lex(diagnostics);

	ASSERT_FALSE(diagnostics.HasErrors());

	ASSERT_GE(tokens.size(), 6);

	EXPECT_EQ(tokens[0].Kind, Assembler::TokenKind::Identifier);
	EXPECT_EQ(tokens[0].Text, "ADD");

	EXPECT_EQ(tokens[1].Kind, Assembler::TokenKind::Identifier);
	EXPECT_EQ(tokens[1].Text, "R1");

	EXPECT_EQ(tokens[2].Kind, Assembler::TokenKind::Identifier);
	EXPECT_EQ(tokens[2].Text, "R1");

	EXPECT_EQ(tokens[3].Kind, Assembler::TokenKind::Identifier);
	EXPECT_EQ(tokens[3].Text, "R0");

	EXPECT_EQ(tokens[4].Kind, Assembler::TokenKind::EndOfLine);
	EXPECT_EQ(tokens[5].Kind, Assembler::TokenKind::EndOfFile);
}

TEST(LexerTests, IgnoresWhitespace)
{
	Assembler::DiagnosticBag diagnostics;
	Assembler::Lexer lexer("   LI    R0     0x1   \n");

	const auto tokens = lexer.Lex(diagnostics);

	ASSERT_FALSE(diagnostics.HasErrors());

	ASSERT_GE(tokens.size(), 5);

	EXPECT_EQ(tokens[0].Kind, Assembler::TokenKind::Identifier);
	EXPECT_EQ(tokens[0].Text, "LI");

	EXPECT_EQ(tokens[1].Kind, Assembler::TokenKind::Identifier);
	EXPECT_EQ(tokens[1].Text, "R0");

	EXPECT_EQ(tokens[2].Kind, Assembler::TokenKind::Number);
	EXPECT_EQ(tokens[2].Text, "0x1");

	EXPECT_EQ(tokens[3].Kind, Assembler::TokenKind::EndOfLine);
	EXPECT_EQ(tokens[4].Kind, Assembler::TokenKind::EndOfFile);
}

TEST(LexerTests, IgnoresComments)
{
	Assembler::DiagnosticBag diagnostics;
	Assembler::Lexer lexer("LI R0 0x1 ; this is a comment\n");

	const auto tokens = lexer.Lex(diagnostics);

	ASSERT_FALSE(diagnostics.HasErrors());

	ASSERT_GE(tokens.size(), 5);

	EXPECT_EQ(tokens[0].Text, "LI");
	EXPECT_EQ(tokens[1].Text, "R0");
	EXPECT_EQ(tokens[2].Text, "0x1");
	EXPECT_EQ(tokens[3].Kind, Assembler::TokenKind::EndOfLine);
	EXPECT_EQ(tokens[4].Kind, Assembler::TokenKind::EndOfFile);
}

TEST(LexerTests, EmitsEndOfLineForEmptyLines)
{
	Assembler::DiagnosticBag diagnostics;
	Assembler::Lexer lexer("\n\n");

	const auto tokens = lexer.Lex(diagnostics);

	ASSERT_FALSE(diagnostics.HasErrors());
	ASSERT_EQ(tokens.size(), 3);
	EXPECT_EQ(tokens[0].Kind, Assembler::TokenKind::EndOfLine);
	EXPECT_EQ(tokens[1].Kind, Assembler::TokenKind::EndOfLine);
	EXPECT_EQ(tokens[2].Kind, Assembler::TokenKind::EndOfFile);
}

TEST(LexerTests, PreservesLineAndColumn)
{
	Assembler::DiagnosticBag diagnostics;
	Assembler::Lexer lexer("\nADD R1 R1 R0\n");

	const auto tokens = lexer.Lex(diagnostics);

	ASSERT_FALSE(diagnostics.HasErrors());

	ASSERT_GE(tokens.size(), 7);

	EXPECT_EQ(tokens[0].Kind, Assembler::TokenKind::EndOfLine);

	EXPECT_EQ(tokens[1].Kind, Assembler::TokenKind::Identifier);
	EXPECT_EQ(tokens[1].Text, "ADD");

	EXPECT_EQ(tokens[1].Range.Begin.Line, 2);
	EXPECT_EQ(tokens[1].Range.Begin.Column, 1);

	EXPECT_EQ(tokens[2].Text, "R1");
	EXPECT_EQ(tokens[2].Range.Begin.Line, 2);
	EXPECT_EQ(tokens[2].Range.Begin.Column, 5);
}

TEST(LexerTests, ReportsUnknownCharacter)
{
	Assembler::DiagnosticBag diagnostics;
	Assembler::Lexer lexer("LI R0 @\n");

	const auto tokens = lexer.Lex(diagnostics);

	EXPECT_TRUE(diagnostics.HasErrors());
	EXPECT_TRUE(hasCode(diagnostics, Assembler::DiagnosticCode::UnknownCharacter));

	ASSERT_FALSE(tokens.empty());
	EXPECT_EQ(tokens.back().Kind, Assembler::TokenKind::EndOfFile);
}
