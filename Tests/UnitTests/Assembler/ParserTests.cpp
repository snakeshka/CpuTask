#include "Lexer.h"
#include "Parser.h"

#include <gtest/gtest.h>

#include <string_view>
#include <vector>

namespace
{
struct ParseResult
{
	Assembler::DiagnosticBag diagnostics;
	std::vector<Assembler::Token> tokens;
	std::vector<Assembler::ParsedInstruction> instructions;
};

ParseResult parseSource(std::string_view source)
{
	ParseResult result;

	Assembler::Lexer lexer(source);
	result.tokens = lexer.Lex(result.diagnostics);

	if (result.diagnostics.HasErrors())
	{
		return result;
	}

	Assembler::Parser parser(result.tokens);
	result.instructions = parser.Parse(result.diagnostics);

	return result;
}

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

TEST(ParserTests, ParsesLiInstruction)
{
	const auto result = parseSource("LI R0 0x1\n");

	ASSERT_FALSE(result.diagnostics.HasErrors());
	ASSERT_EQ(result.instructions.size(), 1);

	const auto &instruction = result.instructions[0];

	EXPECT_EQ(instruction.Mnemonic, "LI");

	ASSERT_EQ(instruction.Operands.size(), 2);

	EXPECT_EQ(instruction.Operands[0].Kind, Assembler::ParsedOperandKind::Identifier);
	EXPECT_EQ(instruction.Operands[0].Text, "R0");

	EXPECT_EQ(instruction.Operands[1].Kind, Assembler::ParsedOperandKind::Number);
	EXPECT_EQ(instruction.Operands[1].Text, "0x1");
}

TEST(ParserTests, ParsesAddInstruction)
{
	const auto result = parseSource("ADD R1 R1 R0\n");

	ASSERT_FALSE(result.diagnostics.HasErrors());
	ASSERT_EQ(result.instructions.size(), 1);

	const auto &instruction = result.instructions[0];

	EXPECT_EQ(instruction.Mnemonic, "ADD");

	ASSERT_EQ(instruction.Operands.size(), 3);

	EXPECT_EQ(instruction.Operands[0].Kind, Assembler::ParsedOperandKind::Identifier);
	EXPECT_EQ(instruction.Operands[0].Text, "R1");

	EXPECT_EQ(instruction.Operands[1].Kind, Assembler::ParsedOperandKind::Identifier);
	EXPECT_EQ(instruction.Operands[1].Text, "R1");

	EXPECT_EQ(instruction.Operands[2].Kind, Assembler::ParsedOperandKind::Identifier);
	EXPECT_EQ(instruction.Operands[2].Text, "R0");
}

TEST(ParserTests, ParsesMultipleInstructions)
{
	const auto result = parseSource(
		"LI R0 0x1\n"
		"LI R1 0x8000\n"
		"ADD R1 R1 R0\n");

	ASSERT_FALSE(result.diagnostics.HasErrors());
	ASSERT_EQ(result.instructions.size(), 3);

	EXPECT_EQ(result.instructions[0].Mnemonic, "LI");
	EXPECT_EQ(result.instructions[1].Mnemonic, "LI");
	EXPECT_EQ(result.instructions[2].Mnemonic, "ADD");

	ASSERT_EQ(result.instructions[0].Operands.size(), 2);
	EXPECT_EQ(result.instructions[0].Operands[0].Text, "R0");
	EXPECT_EQ(result.instructions[0].Operands[1].Text, "0x1");

	ASSERT_EQ(result.instructions[1].Operands.size(), 2);
	EXPECT_EQ(result.instructions[1].Operands[0].Text, "R1");
	EXPECT_EQ(result.instructions[1].Operands[1].Text, "0x8000");

	ASSERT_EQ(result.instructions[2].Operands.size(), 3);
	EXPECT_EQ(result.instructions[2].Operands[0].Text, "R1");
	EXPECT_EQ(result.instructions[2].Operands[1].Text, "R1");
	EXPECT_EQ(result.instructions[2].Operands[2].Text, "R0");
}

TEST(ParserTests, SkipsEmptyLines)
{
	const auto result = parseSource(
		"\n"
		"\n"
		"LI R0 0x1\n"
		"\n"
		"ADD R1 R1 R0\n"
		"\n");

	ASSERT_FALSE(result.diagnostics.HasErrors());
	ASSERT_EQ(result.instructions.size(), 2);

	EXPECT_EQ(result.instructions[0].Mnemonic, "LI");
	EXPECT_EQ(result.instructions[1].Mnemonic, "ADD");
}

TEST(ParserTests, AcceptsFileWithoutTrailingNewline)
{
	const auto result = parseSource("LI R0 0x1");

	ASSERT_FALSE(result.diagnostics.HasErrors());
	ASSERT_EQ(result.instructions.size(), 1);

	EXPECT_EQ(result.instructions[0].Mnemonic, "LI");

	ASSERT_EQ(result.instructions[0].Operands.size(), 2);
	EXPECT_EQ(result.instructions[0].Operands[0].Text, "R0");
	EXPECT_EQ(result.instructions[0].Operands[1].Text, "0x1");
}

TEST(ParserTests, KeepsSourceLocationForMnemonic)
{
	const auto result = parseSource(
		"\n"
		"ADD R1 R1 R0\n");

	ASSERT_FALSE(result.diagnostics.HasErrors());
	ASSERT_EQ(result.instructions.size(), 1);

	const auto &instruction = result.instructions[0];

	EXPECT_EQ(instruction.Mnemonic, "ADD");
	EXPECT_EQ(instruction.Range.Begin.Line, 2);
	EXPECT_EQ(instruction.Range.Begin.Column, 1);
}

TEST(ParserTests, KeepsSourceLocationForOperands)
{
	const auto result = parseSource("ADD R1 R1 R0\n");

	ASSERT_FALSE(result.diagnostics.HasErrors());
	ASSERT_EQ(result.instructions.size(), 1);

	const auto &instruction = result.instructions[0];

	ASSERT_EQ(instruction.Operands.size(), 3);

	EXPECT_EQ(instruction.Operands[0].Text, "R1");
	EXPECT_EQ(instruction.Operands[0].Range.Begin.Line, 1);
	EXPECT_EQ(instruction.Operands[0].Range.Begin.Column, 5);

	EXPECT_EQ(instruction.Operands[1].Text, "R1");
	EXPECT_EQ(instruction.Operands[1].Range.Begin.Line, 1);
	EXPECT_EQ(instruction.Operands[1].Range.Begin.Column, 8);

	EXPECT_EQ(instruction.Operands[2].Text, "R0");
	EXPECT_EQ(instruction.Operands[2].Range.Begin.Line, 1);
	EXPECT_EQ(instruction.Operands[2].Range.Begin.Column, 11);
}

TEST(ParserTests, ReportsUnexpectedTokenWhenLineStartsWithNumber)
{
	const auto result = parseSource("123 R0 0x1\n");

	EXPECT_TRUE(result.diagnostics.HasErrors());
	EXPECT_TRUE(hasCode(result.diagnostics, Assembler::DiagnosticCode::UnexpectedToken));
}

TEST(ParserTests, ReportsUnexpectedTokenForUnknownToken)
{
	const auto result = parseSource("LI R0 @\n");

	EXPECT_TRUE(result.diagnostics.HasErrors());

	EXPECT_TRUE(
		hasCode(result.diagnostics, Assembler::DiagnosticCode::UnknownCharacter) ||
		hasCode(result.diagnostics, Assembler::DiagnosticCode::UnexpectedToken));
}