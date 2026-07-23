#include <Cpu/Isa/Instruction.h>

#include <gtest/gtest.h>

TEST(InstructionTests, ReadsImmediate16AsLittleEndian)
{
	EXPECT_EQ((Cpu::Isa::Instruction{Cpu::Isa::Opcode::LI, 0, 0x01, 0x00}.immediate16()), 0x0001);
	EXPECT_EQ((Cpu::Isa::Instruction{Cpu::Isa::Opcode::LI, 0, 0x00, 0x80}.immediate16()), 0x8000);
	EXPECT_EQ((Cpu::Isa::Instruction{Cpu::Isa::Opcode::LI, 0, 0x34, 0x12}.immediate16()), 0x1234);
	EXPECT_EQ((Cpu::Isa::Instruction{Cpu::Isa::Opcode::LI, 0, 0xFF, 0xFF}.immediate16()), 0xFFFF);
}