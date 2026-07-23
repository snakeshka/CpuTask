#include "Cpu/Isa/Codec.h"

#include "Cpu/Isa/InstructionSet.h"

namespace Cpu::Isa {
InstructionBytes encodeInstruction(const Instruction& instruction)
{
	return {
		static_cast<Byte>(instruction.opcode),
		instruction.a,
		instruction.b,
		instruction.c,
	};
}

std::optional<Instruction> decodeInstruction(InstructionBytes bytes)
{
	const auto opcode = static_cast<Opcode>(bytes[0]);
	if (!findInstructionByOpcode(opcode).has_value()) {
		return std::nullopt;
	}

	return Instruction{opcode, bytes[1], bytes[2], bytes[3]};
}

Bytecode encodeProgram(const std::vector<Instruction>& instructions)
{
	Bytecode bytecode;
	bytecode.reserve(instructions.size() * 4);

	for (const auto& instruction : instructions) {
		const auto bytes = encodeInstruction(instruction);
		bytecode.insert(bytecode.end(), bytes.begin(), bytes.end());
	}

	return bytecode;
}

std::optional<std::vector<Instruction>> decodeProgram(const Bytecode& bytecode)
{
	if (bytecode.size() % 4 != 0) {
		return std::nullopt;
	}

	std::vector<Instruction> instructions;
	instructions.reserve(bytecode.size() / 4);

	for (size_t offset = 0; offset < bytecode.size(); offset += 4) {
		const InstructionBytes bytes = {
			bytecode[offset],
			bytecode[offset + 1],
			bytecode[offset + 2],
			bytecode[offset + 3],
		};

		auto instruction = decodeInstruction(bytes);
		if (!instruction.has_value()) {
			return std::nullopt;
		}

		instructions.push_back(*instruction);
	}

	return instructions;
}
}