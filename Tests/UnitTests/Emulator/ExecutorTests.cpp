#include "CpuState.h"
#include "Executor.h"
#include "Ram.h"

#include <Cpu/Isa/Opcode.h>
#include <gtest/gtest.h>

TEST(ExecutorTests, ExecutesLiInstruction)
{
	Emulator::CpuState state;
	Emulator::Ram ram(65536);
	const Emulator::Executor executor;

	executor.Execute(state, ram, Cpu::Isa::Instruction{Cpu::Isa::Opcode::LI, 1, 0x00, 0x80}, 1);

	EXPECT_EQ(state.GetRegister(1), 0x00008000u);
	EXPECT_EQ(state.InstructionPointer(), 4u);
}

TEST(ExecutorTests, ExecutesAddInstruction)
{
	Emulator::CpuState state;
	state.SetRegister(1, 5);
	state.SetRegister(2, 7);
	Emulator::Ram ram(65536);
	const Emulator::Executor executor;

	executor.Execute(state, ram, Cpu::Isa::Instruction{Cpu::Isa::Opcode::ADD, 3, 1, 2}, 1);

	EXPECT_EQ(state.GetRegister(3), 12u);
	EXPECT_EQ(state.InstructionPointer(), 4u);
}

TEST(ExecutorTests, AddWrapsOnOverflow)
{
	Emulator::CpuState state;
	state.SetRegister(1, 0xFFFFFFFF);
	state.SetRegister(2, 1);
	Emulator::Ram ram(65536);
	const Emulator::Executor executor;

	executor.Execute(state, ram, Cpu::Isa::Instruction{Cpu::Isa::Opcode::ADD, 3, 1, 2}, 1);

	EXPECT_EQ(state.GetRegister(3), 0u);
}

TEST(ExecutorTests, ExecutesLuiInstructionAndPreservesLowerBytes)
{
	Emulator::CpuState state;
	state.SetRegister(1, 0xDEADC0DE);
	Emulator::Ram ram(65536);
	const Emulator::Executor executor;

	executor.Execute(state, ram, Cpu::Isa::Instruction{Cpu::Isa::Opcode::LUI, 1, 0x34, 0x12}, 1);

	EXPECT_EQ(state.GetRegister(1), 0x1234C0DEu);
}

TEST(ExecutorTests, ExecutesSubInstruction)
{
	Emulator::CpuState state;
	state.SetRegister(1, 15);
	state.SetRegister(2, 5);
	Emulator::Ram ram(65536);
	const Emulator::Executor executor;

	executor.Execute(state, ram, Cpu::Isa::Instruction{Cpu::Isa::Opcode::SUB, 3, 1, 2}, 1);

	EXPECT_EQ(state.GetRegister(3), 10u);
}

TEST(ExecutorTests, ExecutesSllInstructionWithMasking)
{
	Emulator::CpuState state;
	state.SetRegister(1, 1);
	state.SetRegister(2, 33);
	Emulator::Ram ram(65536);
	const Emulator::Executor executor;

	executor.Execute(state, ram, Cpu::Isa::Instruction{Cpu::Isa::Opcode::SLL, 3, 1, 2}, 1);

	EXPECT_EQ(state.GetRegister(3), 2u);
}

TEST(ExecutorTests, ExecutesSraInstructionPreservesSign)
{
	Emulator::CpuState state;
	state.SetRegister(1, 0xF0000000);
	state.SetRegister(2, 4);
	Emulator::Ram ram(65536);
	const Emulator::Executor executor;

	executor.Execute(state, ram, Cpu::Isa::Instruction{Cpu::Isa::Opcode::SRA, 3, 1, 2}, 1);

	EXPECT_EQ(state.GetRegister(3), 0xFF000000u);
}

TEST(ExecutorTests, ExecutesSdivInstructionSigned)
{
	Emulator::CpuState state;
	state.SetRegister(1, 0xFFFFFFFB);
	state.SetRegister(2, 0xFFFFFFFF);
	Emulator::Ram ram(65536);
	const Emulator::Executor executor;

	executor.Execute(state, ram, Cpu::Isa::Instruction{Cpu::Isa::Opcode::SDIV, 3, 1, 2}, 1);

	EXPECT_EQ(state.GetRegister(3), 5u);
}

TEST(ExecutorTests, UdivReturnsDivisionByZeroStatus)
{
	Emulator::CpuState state;
	state.SetRegister(1, 10);
	state.SetRegister(2, 0);
	Emulator::Ram ram(65536);
	const Emulator::Executor executor;

	auto status = executor.Execute(state, ram, Cpu::Isa::Instruction{Cpu::Isa::Opcode::UDIV, 3, 1, 2}, 1);

	EXPECT_EQ(status, Emulator::ExecutionStatus::DivisionByZero);
}

TEST(ExecutorTests, SdivReturnsDivisionByZeroStatus)
{
	Emulator::CpuState state;
	state.SetRegister(1, 0xFFFFFFFB);
	state.SetRegister(2, 0);
	Emulator::Ram ram(65536);
	const Emulator::Executor executor;

	auto status = executor.Execute(state, ram, Cpu::Isa::Instruction{Cpu::Isa::Opcode::SDIV, 3, 1, 2}, 1);

	EXPECT_EQ(status, Emulator::ExecutionStatus::DivisionByZero);
}

TEST(ExecutorTests, StatusToStringReturnsCorrectDescriptions)
{
	EXPECT_EQ(Emulator::ToString(Emulator::ExecutionStatus::Success), "Success");
	EXPECT_EQ(Emulator::ToString(Emulator::ExecutionStatus::DivisionByZero), "division by zero");
	EXPECT_EQ(Emulator::ToString(Emulator::ExecutionStatus::IllegalInstruction), "illegal instruction");
}

TEST(ExecutorTests, ExecutesSraInstructionWithZeroShift)
{
	Emulator::CpuState state;
	state.SetRegister(1, 0xF0000000);
	state.SetRegister(2, 0);
	Emulator::Ram ram(65536);
	const Emulator::Executor executor;

	executor.Execute(state, ram, Cpu::Isa::Instruction{Cpu::Isa::Opcode::SRA, 3, 1, 2}, 1);

	EXPECT_EQ(state.GetRegister(3), 0xF0000000u);
}