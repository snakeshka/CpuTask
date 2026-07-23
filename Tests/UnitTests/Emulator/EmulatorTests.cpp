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
		0xFF,
		0x00,
		0x00,
		0x00,
	};

	Emulator::Emulator emulator;

	const auto result = emulator.Run(bytecode);

	EXPECT_FALSE(result.Success);
}