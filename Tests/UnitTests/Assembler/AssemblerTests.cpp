#include "Assembler.h"

#include <Cpu/Common/Types.h>

#include <gtest/gtest.h>

namespace {
bool hasCode(const Assembler::AssembleResult& result, Assembler::DiagnosticCode code)
{
	for (const auto& diagnostic : result.Diagnostics) {
		if (diagnostic.Code == code) {
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

	const auto result = assembler.Assemble("ABC R1 R2 R3\n");

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

// ============================================================================
// Тесты работы с метками (Labels)
// ============================================================================

TEST(AssemblerTests, AssemblesProgramWithForwardAndBackwardJumps)
{
	Assembler::Assembler assembler;

	const auto result = assembler.Assemble(
		"START:\n"
		"  ADD R0 R0 R0\n"   // Адрес 0x00
		"  RJMP END_LABEL\n" // Адрес 0x04, переход на END_LABEL (0x0C), смещение = (0x0C - 0x04) / 4 = +2
		"  LI R1 0x5\n"      // Адрес 0x08
		"END_LABEL:\n"       // Адрес 0x0C
		"  RJMP START\n"     // Адрес 0x0C, переход на START (0x00), смещение = (0x00 - 0x0C) / 4 = -3 (0xFFFD)
		);

	const Cpu::Bytecode expectedBytecode{
		0x02, 0x00, 0x00, 0x00, // ADD R0 R0 R0
		0x30, 0x00, 0x02, 0x00, // RJMP +2
		0x00, 0x01, 0x05, 0x00, // LI R1 0x5
		0x30, 0x00, 0xFD, 0xFF  // RJMP -3
	};

	ASSERT_TRUE(result.Success());
	EXPECT_EQ(result.Bytecode, expectedBytecode);
}

TEST(AssemblerTests, AssemblesLiAndLuiWithLabelAddress)
{
	Assembler::Assembler assembler;

	const auto result = assembler.Assemble(
		"LI R0 TARGET\n"  // Адрес 0x00. Младшая часть адреса TARGET (0x0008 & 0xFFFF = 0x0008)
		"LUI R0 TARGET\n" // Адрес 0x04. Старшая часть адреса TARGET ((0x0008 >> 16) & 0xFFFF = 0x0000)
		"TARGET:\n"       // Адрес 0x08
		"ADD R0 R0 R0\n"  // Адрес 0x08
		);

	const Cpu::Bytecode expectedBytecode{
		0x00, 0x00, 0x08, 0x00, // LI R0 0x0008
		0x01, 0x00, 0x00, 0x00, // LUI R0 0x0000
		0x02, 0x00, 0x00, 0x00  // ADD R0 R0 R0
	};

	ASSERT_TRUE(result.Success());
	EXPECT_EQ(result.Bytecode, expectedBytecode);
}

TEST(AssemblerTests, ReportsDuplicateLabel)
{
	Assembler::Assembler assembler;

	const auto result = assembler.Assemble(
		"MY_LABEL:\n"
		"  ADD R0 R0 R0\n"
		"MY_LABEL:\n"
		"  SUB R0 R0 R0\n"
		);

	EXPECT_FALSE(result.Success());
	EXPECT_TRUE(hasCode(result, Assembler::DiagnosticCode::DuplicateLabel));
}

TEST(AssemblerTests, ReportsUndefinedLabel)
{
	Assembler::Assembler assembler;

	const auto result = assembler.Assemble("RJMP UNKNOWN_LABEL\n");

	EXPECT_FALSE(result.Success());
	EXPECT_TRUE(hasCode(result, Assembler::DiagnosticCode::UndefinedLabel));
}

TEST(AssemblerTests, ReportsInvalidLabelUsage)
{
	Assembler::Assembler assembler;
	// Использование метки в команде ASPI запрещено спецификацией (разрешено только в LI, LUI, RJMP, BRZ, BRNZ)
	const auto result = assembler.Assemble(
		"MY_LABEL:\n"
		"  ASPI R0 MY_LABEL\n"
		);
	EXPECT_FALSE(result.Success());
	EXPECT_TRUE(hasCode(result, Assembler::DiagnosticCode::InvalidLabelUsage));
}