#include "Assembler.h"

#include <Cpu/Common/Types.h>

#include <gtest/gtest.h>

namespace
{
bool hasCode(const Assembler::AssembleResult &result, Assembler::DiagnosticCode code)
{
	for (const auto &diagnostic : result.Diagnostics)
	{
		if (diagnostic.Code == code)
		{
			return true;
		}
	}

	return false;
}

}

TEST(AssemblerTests, AssemblesSimpleProgram)
{
	Assembler::Assembler assembler;

	const auto result = assembler.Assemble(
		"LI R0 0x1\n"
		"LI R1 0x8000\n"
		"ADD R1 R1 R0\n");

	const Cpu::Bytecode expectedBytecode{
		0x00,
		0x00,
		0x01,
		0x00,
		0x00,
		0x01,
		0x00,
		0x80,
		0x02,
		0x01,
		0x01,
		0x00,
	};

	ASSERT_TRUE(result.Success());
	EXPECT_EQ(result.Bytecode, expectedBytecode);
}

TEST(AssemblerTests, AssemblesProgramWithCommentsAndWhitespace)
{
	Assembler::Assembler assembler;

	const auto result = assembler.Assemble(
		"   LI    R0     0x1   ; one\n"
		"\n"
		"LI R1 0x8000\n"
		"ADD R1 R1 R0 ; add\n");

	const Cpu::Bytecode expectedBytecode{
		0x00, 0x00, 0x01, 0x00,
		0x00, 0x01, 0x00, 0x80,
		0x02, 0x01, 0x01, 0x00,
	};

	ASSERT_TRUE(result.Success());
	EXPECT_EQ(result.Bytecode, expectedBytecode);
}

TEST(AssemblerTests, AssemblesEmptySourceToEmptyBytecode)
{
	Assembler::Assembler assembler;

	const auto result = assembler.Assemble("");

	ASSERT_TRUE(result.Success());
	EXPECT_TRUE(result.Bytecode.empty());
}

TEST(AssemblerTests, ReportsUnknownInstruction)
{
	Assembler::Assembler assembler;

	const auto result = assembler.Assemble("MUL R1 R2 R3\n");

	EXPECT_FALSE(result.Success());
	EXPECT_TRUE(hasCode(result, Assembler::DiagnosticCode::UnknownInstruction));
}

TEST(AssemblerTests, ReportsInvalidRegister)
{
	Assembler::Assembler assembler;

	const auto result = assembler.Assemble("ADD R1 R16 R0\n");

	EXPECT_FALSE(result.Success());
	EXPECT_TRUE(hasCode(result, Assembler::DiagnosticCode::InvalidRegister));
}

TEST(AssemblerTests, ReportsImmediateOutOfRange)
{
	Assembler::Assembler assembler;

	const auto result = assembler.Assemble("LI R0 0x10000\n");

	EXPECT_FALSE(result.Success());
	EXPECT_TRUE(hasCode(result, Assembler::DiagnosticCode::ImmediateOutOfRange));
}

TEST(AssemblerTests, ReportsMissingOperand)
{
	Assembler::Assembler assembler;

	const auto result = assembler.Assemble("ADD R1 R2\n");

	EXPECT_FALSE(result.Success());
	EXPECT_TRUE(hasCode(result, Assembler::DiagnosticCode::MissingOperand));
}

TEST(AssemblerTests, ReportsWrongOperandType)
{
	Assembler::Assembler assembler;

	const auto result = assembler.Assemble("LI R1 R2\n");

	EXPECT_FALSE(result.Success());
	EXPECT_TRUE(hasCode(result, Assembler::DiagnosticCode::WrongOperandType));
}
