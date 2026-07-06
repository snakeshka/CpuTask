#include "CpuState.h"

#include <gtest/gtest.h>

TEST(CpuStateTests, InitializesRegistersToZero)
{
	const Emulator::CpuState state;

	for (size_t i = 0; i < 16; ++i)
	{
		EXPECT_EQ(state.GetRegister(i), 0u);
	}
}

TEST(CpuStateTests, InitializesProgramCounterToZero)
{
	const Emulator::CpuState state;

	EXPECT_EQ(state.ProgramCounter(), 0u);
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

	state.AdvanceProgramCounter();
	EXPECT_EQ(state.ProgramCounter(), 4u);

	state.AdvanceProgramCounter();
	EXPECT_EQ(state.ProgramCounter(), 8u);
}
