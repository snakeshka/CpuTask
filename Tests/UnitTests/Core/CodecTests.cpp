#include <Cpu/Isa/Codec.h>
#include <Cpu/Isa/Opcode.h>

#include <gtest/gtest.h>

TEST(CodecTests, EncodesLiInstruction)
{
	EXPECT_EQ(
		Cpu::Isa::encodeInstruction(Cpu::Isa::Instruction{Cpu::Isa::Opcode::LI, 0, 0x01, 0x00}),
		(Cpu::Isa::InstructionBytes{0x00, 0x00, 0x01, 0x00}));
	EXPECT_EQ(
		Cpu::Isa::encodeInstruction(Cpu::Isa::Instruction{Cpu::Isa::Opcode::LI, 1, 0x00, 0x80}),
		(Cpu::Isa::InstructionBytes{0x00, 0x01, 0x00, 0x80}));
}

TEST(CodecTests, EncodesAddInstruction)
{
	EXPECT_EQ(
		Cpu::Isa::encodeInstruction(Cpu::Isa::Instruction{Cpu::Isa::Opcode::ADD, 1, 1, 0}),
		(Cpu::Isa::InstructionBytes{0x02, 0x01, 0x01, 0x00}));
}

TEST(CodecTests, DecodesKnownInstructions)
{
	const auto li = Cpu::Isa::decodeInstruction(Cpu::Isa::InstructionBytes{0x00, 0x01, 0x00, 0x80});
	ASSERT_TRUE(li.has_value());
	EXPECT_EQ(li->opcode, Cpu::Isa::Opcode::LI);
	EXPECT_EQ(li->a, 1);
	EXPECT_EQ(li->b, 0);
	EXPECT_EQ(li->c, 0x80);

	const auto add = Cpu::Isa::decodeInstruction(Cpu::Isa::InstructionBytes{0x02, 0x01, 0x01, 0x00});
	ASSERT_TRUE(add.has_value());
	EXPECT_EQ(add->opcode, Cpu::Isa::Opcode::ADD);
	EXPECT_EQ(add->a, 1);
	EXPECT_EQ(add->b, 1);
	EXPECT_EQ(add->c, 0);
}

TEST(CodecTests, RejectsMalformedProgram)
{
	EXPECT_FALSE(Cpu::Isa::decodeProgram(Cpu::Bytecode{0x00, 0x01, 0x00}).has_value());
	EXPECT_FALSE(Cpu::Isa::decodeInstruction(Cpu::Isa::InstructionBytes{0xFF, 0x00, 0x00, 0x00}).has_value());
}

TEST(CodecTests, EncodesProgramInOrder)
{
	const std::vector<Cpu::Isa::Instruction> instructions{
		{Cpu::Isa::Opcode::LI, 0, 0x01, 0x00},
		{Cpu::Isa::Opcode::LI, 1, 0x00, 0x80},
		{Cpu::Isa::Opcode::ADD, 1, 1, 0},
	};

	const Cpu::Bytecode expected{
		0x00, 0x00, 0x01, 0x00,
		0x00, 0x01, 0x00, 0x80,
		0x02, 0x01, 0x01, 0x00,
	};

	EXPECT_EQ(Cpu::Isa::encodeProgram(instructions), expected);
}