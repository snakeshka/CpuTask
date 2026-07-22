#pragma once

#include "Cpu/Isa/InstructionFormat.h"

#include <optional>
#include <span>
#include <array>
#include <string_view>

namespace Cpu::Isa
{
inline constexpr std::array<OperandKind, 2> RIOperands = {
	OperandKind::Register,
	OperandKind::Immediate16,
};

inline constexpr std::array<OperandKind, 2> RUIOperands = {
	OperandKind::Register,
	OperandKind::UpperImmediate16,
};

inline constexpr std::array<OperandKind, 3> RRROperands = {
	OperandKind::Register,
	OperandKind::Register,
	OperandKind::Register,
};

inline constexpr std::array<OperandKind, 2> SROperands = {
	OperandKind::SystemRegister,
	OperandKind::Register,
};

inline constexpr std::array<OperandKind, 2> RSOperands = {
	OperandKind::Register,
	OperandKind::SystemRegister,
};

inline constexpr std::array<OperandKind, 2> RROperands = {
	OperandKind::Register,
	OperandKind::Register,
};

inline constexpr std::array<OperandKind, 1> ROperands = {
	OperandKind::Register,
};

inline constexpr std::array<OperandKind, 1> IOperands = {
	OperandKind::Immediate16,
};

inline constexpr std::array<InstructionFormat, 36> Instructions = {
	InstructionFormat{"LI", Opcode::LI, RIOperands},
	InstructionFormat{"LUI", Opcode::LUI, RUIOperands},
	InstructionFormat{"ADD", Opcode::ADD, RRROperands},
	InstructionFormat{"SUB", Opcode::SUB, RRROperands},
	InstructionFormat{"AND", Opcode::AND, RRROperands},
	InstructionFormat{"OR", Opcode::OR, RRROperands},
	InstructionFormat{"XOR", Opcode::XOR, RRROperands},
	InstructionFormat{"SLL", Opcode::SLL, RRROperands},
	InstructionFormat{"SRL", Opcode::SRL, RRROperands},
	InstructionFormat{"SRA", Opcode::SRA, RRROperands},
	InstructionFormat{"MUL", Opcode::MUL, RRROperands},
	InstructionFormat{"UDIV", Opcode::UDIV, RRROperands},
	InstructionFormat{"SDIV", Opcode::SDIV, RRROperands},
	InstructionFormat{"LDB", Opcode::LDB, RIOperands},
	InstructionFormat{"LDH", Opcode::LDH, RIOperands},
	InstructionFormat{"LDW", Opcode::LDW, RIOperands},
	InstructionFormat{"STB", Opcode::STB, RIOperands},
	InstructionFormat{"STH", Opcode::STH, RIOperands},
	InstructionFormat{"STW", Opcode::STW, RIOperands},
	InstructionFormat{"SXT", Opcode::SXT, RIOperands},
	InstructionFormat{"BSWAP", Opcode::BSWAP, RIOperands},
	InstructionFormat{"SCRW", Opcode::SCRW, SROperands},
	InstructionFormat{"SCRR", Opcode::SCRR, RSOperands},
	InstructionFormat{"ASPI", Opcode::ASPI, RIOperands},
	InstructionFormat{"ASPR", Opcode::ASPR, RROperands},
	InstructionFormat{"RJMP", Opcode::RJMP, IOperands},
	InstructionFormat{"BRZ", Opcode::BRZ, RIOperands},
	InstructionFormat{"BRNZ", Opcode::BRNZ, RIOperands},
	InstructionFormat{"AJMP", Opcode::AJMP, ROperands},
	InstructionFormat{"CALL", Opcode::CALL, ROperands},
	InstructionFormat{"EQ", Opcode::EQ, RRROperands},
	InstructionFormat{"NE", Opcode::NE, RRROperands},
	InstructionFormat{"LT", Opcode::LT, RRROperands},
	InstructionFormat{"GE", Opcode::GE, RRROperands},
	InstructionFormat{"SLT", Opcode::SLT, RRROperands},
	InstructionFormat{"SGE", Opcode::SGE, RRROperands}};

inline std::span<const InstructionFormat> instructionSet()
{
	return Instructions;
}

constexpr std::optional<InstructionFormat> findInstructionByMnemonic(std::string_view mnemonic)
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

constexpr std::optional<InstructionFormat> findInstructionByOpcode(Opcode opcode)
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
