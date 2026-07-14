#include "Cpu/Isa/InstructionSet.h"

#include <array>
#include <span>
#include <optional>

namespace Cpu::Isa
{

namespace
{

constexpr std::array<OperandKind, 2> LiOperands = {
	OperandKind::Register,
	OperandKind::Immediate16,
};

constexpr std::array<OperandKind, 2> LuiOperands = {
	OperandKind::Register,
	OperandKind::UpperImmediate16,
};

constexpr std::array<OperandKind, 3> AddOperands = {
	OperandKind::Register,
	OperandKind::Register,
	OperandKind::Register,
};

constexpr std::array<OperandKind, 3> SubOperands = {
	OperandKind::Register,
	OperandKind::Register,
	OperandKind::Register,
};

constexpr std::array<OperandKind, 3> AndOperands = {
	OperandKind::Register,
	OperandKind::Register,
	OperandKind::Register,
};

constexpr std::array<OperandKind, 3> OrOperands = {
	OperandKind::Register,
	OperandKind::Register,
	OperandKind::Register,
};

constexpr std::array<OperandKind, 3> XorOperands = {
	OperandKind::Register,
	OperandKind::Register,
	OperandKind::Register,
};

constexpr std::array<OperandKind, 3> SllOperands = {
	OperandKind::Register,
	OperandKind::Register,
	OperandKind::Register,
};

constexpr std::array<OperandKind, 3> SrlOperands = {
	OperandKind::Register,
	OperandKind::Register,
	OperandKind::Register,
};

constexpr std::array<OperandKind, 3> SraOperands = {
	OperandKind::Register,
	OperandKind::Register,
	OperandKind::Register,
};

constexpr std::array<OperandKind, 3> MulOperands = {
	OperandKind::Register,
	OperandKind::Register,
	OperandKind::Register,
};

constexpr std::array<OperandKind, 3> UdivOperands = {
	OperandKind::Register,
	OperandKind::Register,
	OperandKind::Register,
};

constexpr std::array<OperandKind, 3> SdivOperands = {
	OperandKind::Register,
	OperandKind::Register,
	OperandKind::Register,
};

constexpr std::array<OperandKind, 2> LdbOperands = {
	OperandKind::Register,
	OperandKind::Immediate16,
};

constexpr std::array<OperandKind, 2> LdhOperands = {
	OperandKind::Register,
	OperandKind::Immediate16,
};

constexpr std::array<OperandKind, 2> LdwOperands = {
	OperandKind::Register,
	OperandKind::Immediate16,
};

constexpr std::array<OperandKind, 2> StbOperands = {
	OperandKind::Register,
	OperandKind::Immediate16,
};

constexpr std::array<OperandKind, 2> SthOperands = {
	OperandKind::Register,
	OperandKind::Immediate16,
};

constexpr std::array<OperandKind, 2> StwOperands = {
	OperandKind::Register,
	OperandKind::Immediate16,
};

constexpr std::array<OperandKind, 2> SxtOperands = {
	OperandKind::Register,
	OperandKind::Immediate16,
};

constexpr std::array<OperandKind, 2> BswapOperands = {
	OperandKind::Register,
	OperandKind::Immediate16,
};

constexpr std::array<InstructionFormat, 21> Instructions = {
	InstructionFormat{"LI", Opcode::LI, LiOperands},
	InstructionFormat{"LUI", Opcode::LUI, LuiOperands},
	InstructionFormat{"ADD", Opcode::ADD, AddOperands},
	InstructionFormat{"SUB", Opcode::SUB, SubOperands},
	InstructionFormat{"AND", Opcode::AND, AndOperands},
	InstructionFormat{"OR", Opcode::OR, OrOperands},
	InstructionFormat{"XOR", Opcode::XOR, XorOperands},
	InstructionFormat{"SLL", Opcode::SLL, SllOperands},
	InstructionFormat{"SRL", Opcode::SRL, SrlOperands},
	InstructionFormat{"SRA", Opcode::SRA, SraOperands},
	InstructionFormat{"MUL", Opcode::MUL, MulOperands},
	InstructionFormat{"UDIV", Opcode::UDIV, UdivOperands},
	InstructionFormat{"SDIV", Opcode::SDIV, SdivOperands},
	InstructionFormat{"LDB", Opcode::LDB, LdbOperands},
	InstructionFormat{"LDH", Opcode::LDH, LdhOperands},
	InstructionFormat{"LDW", Opcode::LDW, LdwOperands},
	InstructionFormat{"STB", Opcode::STB, StbOperands},
	InstructionFormat{"STH", Opcode::STH, SthOperands},
	InstructionFormat{"STW", Opcode::STW, StwOperands},
	InstructionFormat{"SXT", Opcode::SXT, SxtOperands},
	InstructionFormat{"BSWAP", Opcode::BSWAP, BswapOperands}};

}

std::span<const InstructionFormat> instructionSet()
{
	return Instructions;
}

std::optional<InstructionFormat> findInstructionByMnemonic(std::string_view mnemonic)
{
	for (const auto &instruction : Instructions)
	{
		if (instruction.mnemonic == mnemonic)
		{
			return instruction;
		}
	}

	return std::nullopt;
}

std::optional<InstructionFormat> findInstructionByOpcode(Opcode opcode)
{
	for (const auto &instruction : Instructions)
	{
		if (instruction.opcode == opcode)
		{
			return instruction;
		}
	}

	return std::nullopt;
}

}