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

// Tests/UnitTests/Emulator/ExecutorTests.cpp

// 1. Тест записи и чтения слова (32 бита)
TEST(ExecutorTests, ExecutesStoreAndLoadWord)
{
	Emulator::CpuState state;
	state.SetRegister(1, 0x12345678); // Значение для записи
	state.SetRegister(2, 100);		  // Базовый адрес = 100
	Emulator::Ram ram(65536);
	const Emulator::Executor executor;

	// STW R1 R2 4 -> Записать R1 по адресу R2 + 4 = 104
	auto statusStore = executor.Execute(state, ram, Cpu::Isa::Instruction{Cpu::Isa::Opcode::STW, 1, 2, 4}, 1);
	EXPECT_EQ(statusStore, Emulator::ExecutionStatus::Success);

	// LDW R3 R2 4 -> Прочитать слово по адресу R2 + 4 = 104 в регистр R3
	auto statusLoad = executor.Execute(state, ram, Cpu::Isa::Instruction{Cpu::Isa::Opcode::LDW, 3, 2, 4}, 1);
	EXPECT_EQ(statusLoad, Emulator::ExecutionStatus::Success);

	EXPECT_EQ(state.GetRegister(3), 0x12345678u);
}

// 2. Тест записи и чтения полуслова (16 бит)
TEST(ExecutorTests, ExecutesStoreAndLoadHalfword)
{
	Emulator::CpuState state;
	state.SetRegister(1, 0xABCD); // Младшие 16 бит для записи
	state.SetRegister(2, 200);	  // Базовый адрес = 200
	Emulator::Ram ram(65536);
	const Emulator::Executor executor;

	// STH R1 R2 0 -> Записать младшие 16 бит R1 по адресу R2 (200)
	executor.Execute(state, ram, Cpu::Isa::Instruction{Cpu::Isa::Opcode::STH, 1, 2, 0}, 1);

	// LDH R3 R2 0 -> Прочитать полуслово по адресу R2 (200) в R3
	executor.Execute(state, ram, Cpu::Isa::Instruction{Cpu::Isa::Opcode::LDH, 3, 2, 0}, 1);

	EXPECT_EQ(state.GetRegister(3), 0x0000ABCDu); // Верхние 16 бит должны быть 0
}

// 3. Тест записи и чтения байта (8 бит)
TEST(ExecutorTests, ExecutesStoreAndLoadByte)
{
	Emulator::CpuState state;
	state.SetRegister(1, 0xEF); // Младшие 8 бит для записи
	state.SetRegister(2, 300);	// Базовый адрес = 300
	Emulator::Ram ram(65536);
	const Emulator::Executor executor;

	// STB R1 R2 10 -> Записать младшие 8 бит R1 по адресу R2 + 10 = 310
	executor.Execute(state, ram, Cpu::Isa::Instruction{Cpu::Isa::Opcode::STB, 1, 2, 10}, 1);

	// LDB R3 R2 10 -> Прочитать байт по адресу R2 + 10 = 310 в R3
	executor.Execute(state, ram, Cpu::Isa::Instruction{Cpu::Isa::Opcode::LDB, 3, 2, 10}, 1);

	EXPECT_EQ(state.GetRegister(3), 0x000000EFu); // Верхние 24 бита должны быть 0
}

// 4. Тест знакового расширения SXT
TEST(ExecutorTests, ExecutesSignExtensionSxt)
{
	Emulator::CpuState state;
	Emulator::Ram ram(65536);
	const Emulator::Executor executor;

	// Тест А: Знаковое расширение отрицательного байта (I = 0, 0x80 -> 0xFFFFFF80)
	state.SetRegister(1, 0x80);
	executor.Execute(state, ram, Cpu::Isa::Instruction{Cpu::Isa::Opcode::SXT, 2, 1, 0}, 1);
	EXPECT_EQ(state.GetRegister(2), 0xFFFFFF80u);

	// Тест Б: Знаковое расширение положительного байта (I = 0, 0x7F -> 0x0000007F)
	state.SetRegister(1, 0x7F);
	executor.Execute(state, ram, Cpu::Isa::Instruction{Cpu::Isa::Opcode::SXT, 2, 1, 0}, 1);
	EXPECT_EQ(state.GetRegister(2), 0x0000007Fu);

	// Тест В: Знаковое расширение отрицательного полуслова (I = 1, 0x8000 -> 0xFFFF8000)
	state.SetRegister(1, 0x8000);
	executor.Execute(state, ram, Cpu::Isa::Instruction{Cpu::Isa::Opcode::SXT, 2, 1, 1}, 1);
	EXPECT_EQ(state.GetRegister(2), 0xFFFF8000u);
}

// 5. Тест перестановки байт BSWAP
TEST(ExecutorTests, ExecutesByteSwapBswap)
{
	Emulator::CpuState state;
	Emulator::Ram ram(65536);
	const Emulator::Executor executor;

	// Тест А: Своп байт внутри младшего полуслова (I = 1, 0x12345678 -> 0x12347856)
	state.SetRegister(1, 0x12345678);
	executor.Execute(state, ram, Cpu::Isa::Instruction{Cpu::Isa::Opcode::BSWAP, 2, 1, 1}, 1);
	EXPECT_EQ(state.GetRegister(2), 0x12347856u);

	// Тест Б: Своп байт внутри всего слова (I = 2, 0x12345678 -> 0x78563412)
	state.SetRegister(1, 0x12345678);
	executor.Execute(state, ram, Cpu::Isa::Instruction{Cpu::Isa::Opcode::BSWAP, 2, 1, 2}, 1);
	EXPECT_EQ(state.GetRegister(2), 0x78563412u);
}