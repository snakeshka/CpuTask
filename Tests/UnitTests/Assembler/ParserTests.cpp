#include "Lexer.h"
#include "Parser.h"

#include <gtest/gtest.h>

#include <string_view>
#include <vector>

namespace {
struct ParseResult
{
	Assembler::DiagnosticBag diagnostics;
	std::vector<Assembler::Token> tokens;
	std::vector<Assembler::Statement> statements; // <-- Раньше было ParsedInstruction
};

ParseResult parseSource(std::string_view source)
{
	ParseResult result;

	Assembler::Lexer lexer(source);
	result.tokens = lexer.Lex(result.diagnostics);

	if (result.diagnostics.HasErrors()) {
		return result;
	}

	Assembler::Parser parser(result.tokens);
	result.statements = parser.Parse(result.diagnostics);

	return result;
}

bool hasCode(const Assembler::DiagnosticBag& diagnostics, Assembler::DiagnosticCode code)
{
	for (const auto& diagnostic : diagnostics.diagnostics()) {
		if (diagnostic.Code == code) {
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
	ASSERT_EQ(result.statements.size(), 1);

	const auto& instruction = result.statements[0].Instruction;

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
	ASSERT_EQ(result.statements.size(), 1);

	const auto& instruction = result.statements[0].Instruction;

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
	ASSERT_EQ(result.statements.size(), 3);

	EXPECT_EQ(result.statements[0].Instruction.Mnemonic, "LI");
	EXPECT_EQ(result.statements[1].Instruction.Mnemonic, "LI");
	EXPECT_EQ(result.statements[2].Instruction.Mnemonic, "ADD");

	ASSERT_EQ(result.statements[0].Instruction.Operands.size(), 2);
	EXPECT_EQ(result.statements[0].Instruction.Operands[0].Text, "R0");
	EXPECT_EQ(result.statements[0].Instruction.Operands[1].Text, "0x1");

	ASSERT_EQ(result.statements[1].Instruction.Operands.size(), 2);
	EXPECT_EQ(result.statements[1].Instruction.Operands[0].Text, "R1");
	EXPECT_EQ(result.statements[1].Instruction.Operands[1].Text, "0x8000");

	ASSERT_EQ(result.statements[2].Instruction.Operands.size(), 3);
	EXPECT_EQ(result.statements[2].Instruction.Operands[0].Text, "R1");
	EXPECT_EQ(result.statements[2].Instruction.Operands[1].Text, "R1");
	EXPECT_EQ(result.statements[2].Instruction.Operands[2].Text, "R0");
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
	ASSERT_EQ(result.statements.size(), 2);

	EXPECT_EQ(result.statements[0].Instruction.Mnemonic, "LI");
	EXPECT_EQ(result.statements[1].Instruction.Mnemonic, "ADD");
}

TEST(ParserTests, AcceptsFileWithoutTrailingNewline)
{
	const auto result = parseSource("LI R0 0x1");

	ASSERT_FALSE(result.diagnostics.HasErrors());
	ASSERT_EQ(result.statements.size(), 1);

	EXPECT_EQ(result.statements[0].Instruction.Mnemonic, "LI");

	ASSERT_EQ(result.statements[0].Instruction.Operands.size(), 2);
	EXPECT_EQ(result.statements[0].Instruction.Operands[0].Text, "R0");
	EXPECT_EQ(result.statements[0].Instruction.Operands[1].Text, "0x1");
}

TEST(ParserTests, KeepsSourceLocationForMnemonic)
{
	const auto result = parseSource(
		"\n"
		"ADD R1 R1 R0\n");

	ASSERT_FALSE(result.diagnostics.HasErrors());
	ASSERT_EQ(result.statements.size(), 1);

	const auto& instruction = result.statements[0].Instruction;

	EXPECT_EQ(instruction.Mnemonic, "ADD");
	EXPECT_EQ(instruction.Range.Begin.Line, 2);
	EXPECT_EQ(instruction.Range.Begin.Column, 1);
}

TEST(ParserTests, KeepsSourceLocationForOperands)
{
	const auto result = parseSource("ADD R1 R1 R0\n");

	ASSERT_FALSE(result.diagnostics.HasErrors());
	ASSERT_EQ(result.statements.size(), 1);

	const auto& instruction = result.statements[0].Instruction;

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

TEST(ParserTests, ParsesLabelDefinition)
{
	const auto result = parseSource(
		"MY_LABEL:\n"
		"ADD R0 R0 R0\n"
		);

	ASSERT_FALSE(result.diagnostics.HasErrors());
	ASSERT_EQ(result.statements.size(), 2);

	// Проверка метки
	EXPECT_EQ(result.statements[0].Kind, Assembler::StatementKind::Label);
	EXPECT_EQ(result.statements[0].LabelName, "MY_LABEL");

	// Проверка следующей инструкции
	EXPECT_EQ(result.statements[1].Kind, Assembler::StatementKind::Instruction);
	EXPECT_EQ(result.statements[1].Instruction.Mnemonic, "ADD");
}

TEST(ParserTests, ParsesInstructionWithLabelOperand)
{
	const auto result = parseSource("RJMP TARGET_LABEL\n");

	ASSERT_FALSE(result.diagnostics.HasErrors());
	ASSERT_EQ(result.statements.size(), 1);
	ASSERT_EQ(result.statements[0].Kind, Assembler::StatementKind::Instruction);

	const auto& instruction = result.statements[0].Instruction;
	EXPECT_EQ(instruction.Mnemonic, "RJMP");
	ASSERT_EQ(instruction.Operands.size(), 1);
	EXPECT_EQ(instruction.Operands[0].Kind, Assembler::ParsedOperandKind::Identifier);
	EXPECT_EQ(instruction.Operands[0].Text, "TARGET_LABEL");
}