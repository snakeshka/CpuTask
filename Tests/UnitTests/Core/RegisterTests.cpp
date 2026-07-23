#include <Cpu/Isa/Register.h>

#include <gtest/gtest.h>

TEST(RegisterTests, ParsesValidRegisters)
{
	const auto r0 = Cpu::Isa::parseRegister("R0");
	const auto r1 = Cpu::Isa::parseRegister("R1");
	const auto r15 = Cpu::Isa::parseRegister("R15");

	ASSERT_TRUE(r0.has_value());
	ASSERT_TRUE(r1.has_value());
	ASSERT_TRUE(r15.has_value());
	EXPECT_EQ(*r0, 0);
	EXPECT_EQ(*r1, 1);
	EXPECT_EQ(*r15, 15);
}

TEST(RegisterTests, RejectsInvalidRegisters)
{
	EXPECT_FALSE(Cpu::Isa::parseRegister("R16").has_value());
	EXPECT_FALSE(Cpu::Isa::parseRegister("R999").has_value());
	EXPECT_FALSE(Cpu::Isa::parseRegister("RX").has_value());
	EXPECT_FALSE(Cpu::Isa::parseRegister("R").has_value());
	EXPECT_FALSE(Cpu::Isa::parseRegister("").has_value());
}

TEST(RegisterTests, ChecksRegisterIndexValidity)
{
	EXPECT_TRUE(Cpu::Isa::isValidRegister(0));
	EXPECT_TRUE(Cpu::Isa::isValidRegister(15));
	EXPECT_FALSE(Cpu::Isa::isValidRegister(16));
}