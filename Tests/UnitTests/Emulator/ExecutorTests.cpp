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

TEST(ExecutorTests, ExecutesLuiInstructionAndPreservesLowerBytes)
{
	Emulator::CpuState state;
	state.SetRegister(1, 0xDEADC0DE); // Младшие байты C0DE должны сохраниться
	const Emulator::Executor executor;

	// LUI R1 0x1234
	executor.Execute(state, Cpu::Isa::Instruction{Cpu::Isa::Opcode::LUI, 1, 0x34, 0x12});

	EXPECT_EQ(state.GetRegister(1), 0x1234C0DEu);
}

TEST(ExecutorTests, ExecutesSubInstruction)
{
	Emulator::CpuState state;
	state.SetRegister(1, 15);
	state.SetRegister(2, 5);
	const Emulator::Executor executor;

	// SUB R3 R1 R2
	executor.Execute(state, Cpu::Isa::Instruction{Cpu::Isa::Opcode::SUB, 3, 1, 2});

	EXPECT_EQ(state.GetRegister(3), 10u);
}

TEST(ExecutorTests, ExecutesSllInstructionWithMasking)
{
	Emulator::CpuState state;
	state.SetRegister(1, 1);
	state.SetRegister(2, 33); // 33 & 0x1F = 1
	const Emulator::Executor executor;

	// SLL R3 R1 R2
	executor.Execute(state, Cpu::Isa::Instruction{Cpu::Isa::Opcode::SLL, 3, 1, 2});

	EXPECT_EQ(state.GetRegister(3), 2u); // 1 << 1 = 2
}

TEST(ExecutorTests, ExecutesSraInstructionPreservesSign)
{
	Emulator::CpuState state;
	state.SetRegister(1, 0xF0000000); // Отрицательное число
	state.SetRegister(2, 4);
	const Emulator::Executor executor;

	// SRA R3 R1 R2
	executor.Execute(state, Cpu::Isa::Instruction{Cpu::Isa::Opcode::SRA, 3, 1, 2});

	EXPECT_EQ(state.GetRegister(3), 0xFF000000u); // Знак сохранился
}

TEST(ExecutorTests, ExecutesSdivInstructionSigned)
{
	Emulator::CpuState state;
	state.SetRegister(1, 0xFFFFFFFB); // -5
	state.SetRegister(2, 0xFFFFFFFF); // -1
	const Emulator::Executor executor;

	// SDIV R3 R1 R2
	executor.Execute(state, Cpu::Isa::Instruction{Cpu::Isa::Opcode::SDIV, 3, 1, 2});

	EXPECT_EQ(state.GetRegister(3), 5u); // -5 / -1 = 5
}

TEST(ExecutorTests, UdivThrowsOnDivisionByZero)
{
	Emulator::CpuState state;
	state.SetRegister(1, 10);
	state.SetRegister(2, 0); // Делитель равен нулю
	const Emulator::Executor executor;

	// UDIV R3 R1 R2
	EXPECT_THROW(
		executor.Execute(state, Cpu::Isa::Instruction{Cpu::Isa::Opcode::UDIV, 3, 1, 2}),
		std::runtime_error);
}

TEST(ExecutorTests, ExecutesSraInstructionWithZeroShift)
{
	Emulator::CpuState state;
	state.SetRegister(1, 0xF0000000);
	state.SetRegister(2, 0); // сдвиг на 0 бит
	const Emulator::Executor executor;

	// SRA R3 R1 R2
	executor.Execute(state, Cpu::Isa::Instruction{Cpu::Isa::Opcode::SRA, 3, 1, 2});

	EXPECT_EQ(state.GetRegister(3), 0xF0000000u); // Число не должно измениться
}