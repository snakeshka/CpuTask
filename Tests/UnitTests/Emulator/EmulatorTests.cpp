#include "Emulator.h"

#include <gtest/gtest.h>

TEST(EmulatorTests, RunsSimpleProgram)
{
	const Cpu::Bytecode bytecode{
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

	Emulator::Emulator emulator;

	const auto result = emulator.Run(bytecode);

	ASSERT_TRUE(result.Success);
	EXPECT_EQ(result.State.GetRegister(0), 0x00000001);
	EXPECT_EQ(result.State.GetRegister(1), 0x00008001);
	EXPECT_EQ(result.State.InstructionPointer(), 12u);

	for (size_t i = 2; i < 16; ++i) {
		EXPECT_EQ(result.State.GetRegister(i), 0x00000000);
	}
}

TEST(EmulatorTests, RejectsInvalidBytecodeSize)
{
	const Cpu::Bytecode bytecode{
		0x00,
		0x00,
		0x01,
	};

	Emulator::Emulator emulator;

	const auto result = emulator.Run(bytecode);

	EXPECT_FALSE(result.Success);
}

TEST(EmulatorTests, RejectsUnknownOpcode)
{
	const Cpu::Bytecode bytecode{
		0xFE,
		0x00,
		0x00,
		0x00,
	};

	Emulator::Emulator emulator;

	const auto result = emulator.Run(bytecode);

	EXPECT_FALSE(result.Success);
}

TEST(EmulatorTests, HandlesDivisionByZeroInterrupt)
{
	// Программа:
	// 0x00: LI R2 0x100         (R2 = 0x100)
	// 0x04: SCRW IHR (5) R2     (Устанавливаем обработчик IHR = 0x100)
	// 0x08: LI R1 0             (R1 = 0)
	// 0x0C: UDIV R0 R0 R1       (Деление на 0! Прерывание на адресе 0x0C!)
	const Cpu::Bytecode bytecode{
		0x00, 0x02, 0x00, 0x01, // LI R2 0x100
		0x20, 0x05, 0x02, 0x00, // SCRW IHR (5) R2
		0x00, 0x01, 0x00, 0x00, // LI R1 0
		0x0B, 0x00, 0x00, 0x01  // UDIV R0 R0 R1
	};

	Emulator::Emulator emulator;
	Cpu::Bytecode program = bytecode;
	program.resize(0x100, 0x00);

	const auto result = emulator.Run(program);

	EXPECT_EQ(result.State.GetScr(3), 0x0Cu); // IIP = 0x0C (адрес инструкции UDIV)
	EXPECT_EQ(result.State.GetScr(4), 1u);    // ICR = 1 (Illegal Operand)
}

TEST(EmulatorTests, HandlesStackFaultInterrupt)
{
	// Программа:
	// 0x00: LI R1 10            (R1 = 10)
	// 0x04: SCRW SP_SIZE (2) R1 (SP_SIZE = 10)
	// 0x08: LI R2 0x100         (R2 = 0x100)
	// 0x0C: SCRW IHR (5) R2     (IHR = 0x100)
	// 0x10: ASPI R0 -50         (Запрашиваем 50 байт, превышая 10. Stack Fault на адресе 0x10!)
	const Cpu::Bytecode bytecode{
		0x00, 0x01, 0x0A, 0x00, // LI R1 10
		0x20, 0x02, 0x01, 0x00, // SCRW SP_SIZE (2) R1
		0x00, 0x02, 0x00, 0x01, // LI R2 0x100
		0x20, 0x05, 0x02, 0x00, // SCRW IHR (5) R2
		0x22, 0x00, 0xCE, 0xFF  // ASPI R0 -50 (0xFFCE = -50)
	};

	Emulator::Emulator emulator;
	Cpu::Bytecode program = bytecode;
	program.resize(0x100, 0x00);

	const auto result = emulator.Run(program);

	EXPECT_EQ(result.State.GetScr(3), 0x10u); // IIP = 0x10 (адрес инструкции ASPI)
	EXPECT_EQ(result.State.GetScr(4), 2u);    // ICR = 2 (Stack Fault)
}