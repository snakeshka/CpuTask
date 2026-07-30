#include "CpuState.h"

#include <gtest/gtest.h>

TEST(CpuStateTests, InitializesRegistersToZero)
{
	const Emulator::CpuState state;

	for (size_t i = 0; i < 16; ++i) {
		EXPECT_EQ(state.GetRegister(i), 0u);
	}
}

TEST(CpuStateTests, InitializesProgramCounterToZero)
{
	const Emulator::CpuState state;

	EXPECT_EQ(state.InstructionPointer(), 0u);
}

TEST(CpuStateTests, SetsAndGetsRegisters)
{
	Emulator::CpuState state;

	state.SetRegister(0, 123);
	state.SetRegister(15, 0xFFFFFFFF);

	EXPECT_EQ(state.GetRegister(0), 123u);
	EXPECT_EQ(state.GetRegister(15), 0xFFFFFFFFu);
}

TEST(CpuStateTests, AdvancesProgramCounterByInstructionSize)
{
	Emulator::CpuState state;

	state.AdvanceInstructionPointer();
	EXPECT_EQ(state.InstructionPointer(), 4u);

	state.AdvanceInstructionPointer();
	EXPECT_EQ(state.InstructionPointer(), 8u);
}

TEST(CpuStateTests, HandlesInterruptSystemRegisters)
{
	Emulator::CpuState state;
	// SCR 3: IIP, SCR 4: ICR, SCR 5: IHR
	state.SetScr(3, 0x1004); // IIP
	state.SetScr(4, 1);      // ICR (Illegal Operand)
	state.SetScr(5, 0x8000); // IHR
	EXPECT_EQ(state.GetScr(3), 0x1004u);
	EXPECT_EQ(state.GetScr(4), 1u);
	EXPECT_EQ(state.GetScr(5), 0x8000u);
}

TEST(CpuStateTests, ValidatesScrIndices)
{
	Emulator::CpuState state;
	EXPECT_TRUE(state.IsValidScr(0));  // IP
	EXPECT_TRUE(state.IsValidScr(1));  // SP_TOP
	EXPECT_TRUE(state.IsValidScr(2));  // SP_SIZE
	EXPECT_TRUE(state.IsValidScr(3));  // IIP
	EXPECT_TRUE(state.IsValidScr(4));  // ICR
	EXPECT_TRUE(state.IsValidScr(5));  // IHR
	EXPECT_FALSE(state.IsValidScr(6)); // Несуществующий регистр
	EXPECT_FALSE(state.IsValidScr(10));
}