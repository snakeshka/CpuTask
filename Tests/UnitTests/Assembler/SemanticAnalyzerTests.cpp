#include "Lexer.h"
#include "Parser.h"
#include "SemanticAnalyzer.h"
#include "Tests/TestUtils.h"

#include <Cpu/Isa/Opcode.h>

#include <gtest/gtest.h>

#include <string_view>
#include <vector>

namespace
{

struct AnalyzeResult
{
	Assembler::DiagnosticBag diagnostics;
	std::vector<Cpu::Isa::Instruction> instructions;
};

AnalyzeResult analyzeSource(std::string_view source)
{
	AnalyzeResult result;

	Assembler::Lexer lexer(source);
	const auto tokens = lexer.Lex(result.diagnostics);
	if (result.diagnostics.HasErrors())
	{
		return result;
	}

	Assembler::Parser parser(tokens);
	const auto parsed = parser.Parse(result.diagnostics);
	if (result.diagnostics.HasErrors())
	{
		return result;
	}

	Assembler::SemanticAnalyzer analyzer;
	result.instructions = analyzer.Analyze(parsed, result.diagnostics);
	return result;
}

}

TEST(SemanticAnalyzerTests, BuildsLiInstruction)
{
	const auto result = analyzeSource("LI R0 0x1\n");

	ASSERT_FALSE(result.diagnostics.HasErrors());
	ASSERT_EQ(result.instructions.size(), 1);
	EXPECT_EQ(result.instructions[0].opcode, Cpu::Isa::Opcode::LI);
	EXPECT_EQ(result.instructions[0].a, 0);
	EXPECT_EQ(result.instructions[0].b, 0x01);
	EXPECT_EQ(result.instructions[0].c, 0x00);
}

TEST(SemanticAnalyzerTests, BuildsLiInstructionWithHighByteImmediate)
{
	const auto result = analyzeSource("LI R1 0x8000\n");

	ASSERT_FALSE(result.diagnostics.HasErrors());
	ASSERT_EQ(result.instructions.size(), 1);
	EXPECT_EQ(result.instructions[0].opcode, Cpu::Isa::Opcode::LI);
	EXPECT_EQ(result.instructions[0].a, 1);
	EXPECT_EQ(result.instructions[0].b, 0x00);
	EXPECT_EQ(result.instructions[0].c, 0x80);
}

TEST(SemanticAnalyzerTests, BuildsAddInstruction)
{
	const auto result = analyzeSource("ADD R1 R1 R0\n");

	ASSERT_FALSE(result.diagnostics.HasErrors());
	ASSERT_EQ(result.instructions.size(), 1);
	EXPECT_EQ(result.instructions[0].opcode, Cpu::Isa::Opcode::ADD);
	EXPECT_EQ(result.instructions[0].a, 1);
	EXPECT_EQ(result.instructions[0].b, 1);
	EXPECT_EQ(result.instructions[0].c, 0);
}

TEST(SemanticAnalyzerTests, ReportsUnknownInstruction)
{
	const auto result = analyzeSource("ABC R1 R2 R3\n");

	EXPECT_TRUE(Tests::HasDiagnosticCode(result.diagnostics, Assembler::DiagnosticCode::UnknownInstruction));
}

TEST(SemanticAnalyzerTests, ReportsInvalidRegister)
{
	const auto result = analyzeSource("ADD R1 R16 R0\n");

	EXPECT_TRUE(Tests::HasDiagnosticCode(result.diagnostics, Assembler::DiagnosticCode::InvalidRegister));
}

TEST(SemanticAnalyzerTests, ReportsImmediateOutOfRange)
{
	const auto result = analyzeSource("LI R0 0x10000\n");

	EXPECT_TRUE(Tests::HasDiagnosticCode(result.diagnostics, Assembler::DiagnosticCode::ImmediateOutOfRange));
}

TEST(SemanticAnalyzerTests, ReportsMissingOperand)
{
	const auto result = analyzeSource("ADD R1 R2\n");

	EXPECT_TRUE(Tests::HasDiagnosticCode(result.diagnostics, Assembler::DiagnosticCode::MissingOperand));
}

TEST(SemanticAnalyzerTests, ReportsTooManyOperands)
{
	const auto result = analyzeSource("ADD R1 R2 R3 R4\n");

	EXPECT_TRUE(Tests::HasDiagnosticCode(result.diagnostics, Assembler::DiagnosticCode::TooManyOperands));
}

TEST(SemanticAnalyzerTests, ReportsWrongOperandTypeForImmediateRegister)
{
	const auto result = analyzeSource("LI R1 R2\n");

	EXPECT_TRUE(Tests::HasDiagnosticCode(result.diagnostics, Assembler::DiagnosticCode::WrongOperandType));
}

TEST(SemanticAnalyzerTests, ReportsWrongOperandTypeForRegisterImmediate)
{
	const auto result = analyzeSource("ADD R1 R2 5\n");

	EXPECT_TRUE(Tests::HasDiagnosticCode(result.diagnostics, Assembler::DiagnosticCode::WrongOperandType));
}

TEST(SemanticAnalyzerTests, ReportsInvalidImmediateOrWrongOperandType)
{
	const auto result = analyzeSource("LI R0 abc\n");

	EXPECT_TRUE(
		Tests::HasDiagnosticCode(result.diagnostics, Assembler::DiagnosticCode::InvalidImmediate) ||
		Tests::HasDiagnosticCode(result.diagnostics, Assembler::DiagnosticCode::WrongOperandType));
}
