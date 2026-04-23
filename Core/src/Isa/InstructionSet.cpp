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

constexpr std::array<OperandKind, 3> AddOperands = {
	OperandKind::Register,
	OperandKind::Register,
	OperandKind::Register,
};

constexpr std::array<InstructionFormat, 2> Instructions = {
	InstructionFormat{"LI", Opcode::LI, LiOperands},
	InstructionFormat{"ADD", Opcode::ADD, AddOperands},
};

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