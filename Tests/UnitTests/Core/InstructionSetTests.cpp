#include <Cpu/Isa/InstructionSet.h>
#include <Cpu/Isa/Opcode.h>
#include <Cpu/Isa/OperandKind.h>

#include <gtest/gtest.h>

TEST(InstructionSetTests, FindsLiInstruction)
{
	const auto instruction = Cpu::Isa::findInstructionByMnemonic("LI");

	ASSERT_TRUE(instruction.has_value());
	EXPECT_EQ(instruction->mnemonic, "LI");
	EXPECT_EQ(instruction->opcode, Cpu::Isa::Opcode::LI);

	ASSERT_EQ(instruction->operands.size(), 2);
	EXPECT_EQ(instruction->operands[0], Cpu::Isa::OperandKind::Register);
	EXPECT_EQ(instruction->operands[1], Cpu::Isa::OperandKind::Immediate16);
}

TEST(InstructionSetTests, FindsLuiInstruction)
{
	const auto instruction = Cpu::Isa::findInstructionByMnemonic("LUI");

	ASSERT_TRUE(instruction.has_value());
	EXPECT_EQ(instruction->mnemonic, "LUI");
	EXPECT_EQ(instruction->opcode, Cpu::Isa::Opcode::LUI);
	ASSERT_EQ(instruction->operands.size(), 2);
	EXPECT_EQ(instruction->operands[0], Cpu::Isa::OperandKind::Register);
	EXPECT_EQ(instruction->operands[1], Cpu::Isa::OperandKind::UpperImmediate16);
}


TEST(InstructionSetTests, FindsAddInstruction)
{
	const auto instruction = Cpu::Isa::findInstructionByMnemonic("ADD");

	ASSERT_TRUE(instruction.has_value());
	EXPECT_EQ(instruction->mnemonic, "ADD");
	EXPECT_EQ(instruction->opcode, Cpu::Isa::Opcode::ADD);

	ASSERT_EQ(instruction->operands.size(), 3);
	EXPECT_EQ(instruction->operands[0], Cpu::Isa::OperandKind::Register);
	EXPECT_EQ(instruction->operands[1], Cpu::Isa::OperandKind::Register);
	EXPECT_EQ(instruction->operands[2], Cpu::Isa::OperandKind::Register);
}

TEST(InstructionSetTests, FindsSubInstruction)
{
	const auto instruction = Cpu::Isa::findInstructionByMnemonic("SUB");

	ASSERT_TRUE(instruction.has_value());
	EXPECT_EQ(instruction->mnemonic, "SUB");
	EXPECT_EQ(instruction->opcode, Cpu::Isa::Opcode::SUB);
	ASSERT_EQ(instruction->operands.size(), 3);
	EXPECT_EQ(instruction->operands[0], Cpu::Isa::OperandKind::Register);
	EXPECT_EQ(instruction->operands[1], Cpu::Isa::OperandKind::Register);
	EXPECT_EQ(instruction->operands[2], Cpu::Isa::OperandKind::Register);
}

TEST(InstructionSetTests, FindsSraInstruction)
{
	const auto instruction = Cpu::Isa::findInstructionByMnemonic("SRA");

	ASSERT_TRUE(instruction.has_value());
	EXPECT_EQ(instruction->mnemonic, "SRA");
	EXPECT_EQ(instruction->opcode, Cpu::Isa::Opcode::SRA);
}

TEST(InstructionSetTests, FindsSdivInstruction)
{
	const auto instruction = Cpu::Isa::findInstructionByMnemonic("SDIV");

	ASSERT_TRUE(instruction.has_value());
	EXPECT_EQ(instruction->mnemonic, "SDIV");
	EXPECT_EQ(instruction->opcode, Cpu::Isa::Opcode::SDIV);
}

TEST(InstructionSetTests, ReturnsNulloptForUnknownInstruction)
{
	const auto instruction = Cpu::Isa::findInstructionByMnemonic("ABC");

	EXPECT_FALSE(instruction.has_value());
}

TEST(InstructionSetTests, FindsInstructionByOpcode)
{
	const auto li = Cpu::Isa::findInstructionByOpcode(Cpu::Isa::Opcode::LI);

	ASSERT_TRUE(li.has_value());
	EXPECT_EQ(li->mnemonic, "LI");
	EXPECT_EQ(li->opcode, Cpu::Isa::Opcode::LI);

	const auto add = Cpu::Isa::findInstructionByOpcode(Cpu::Isa::Opcode::ADD);

	ASSERT_TRUE(add.has_value());
	EXPECT_EQ(add->mnemonic, "ADD");
	EXPECT_EQ(add->opcode, Cpu::Isa::Opcode::ADD);
}