#include "CpuState.h"
#include "Executor.h"

#include <Cpu/Isa/Opcode.h>

#include <gtest/gtest.h>

TEST(ExecutorTests, ExecutesLiInstruction)
{
	Emulator::CpuState state;
	const Emulator::Executor executor;

	executor.Execute(state, Cpu::Isa::Instruction{Cpu::Isa::Opcode::LI, 1, 0x00, 0x80});

	EXPECT_EQ(state.GetRegister(1), 0x00008000u);
	EXPECT_EQ(state.ProgramCounter(), 4u);
}

TEST(ExecutorTests, ExecutesAddInstruction)
{
	Emulator::CpuState state;
	state.SetRegister(1, 5);
	state.SetRegister(2, 7);
	const Emulator::Executor executor;

	executor.Execute(state, Cpu::Isa::Instruction{Cpu::Isa::Opcode::ADD, 3, 1, 2});

	EXPECT_EQ(state.GetRegister(3), 12u);
	EXPECT_EQ(state.ProgramCounter(), 4u);
}

TEST(ExecutorTests, AddWrapsOnOverflow)
{
	Emulator::CpuState state;
	state.SetRegister(1, 0xFFFFFFFF);
	state.SetRegister(2, 1);
	const Emulator::Executor executor;

	executor.Execute(state, Cpu::Isa::Instruction{Cpu::Isa::Opcode::ADD, 3, 1, 2});

	EXPECT_EQ(state.GetRegister(3), 0u);
}
