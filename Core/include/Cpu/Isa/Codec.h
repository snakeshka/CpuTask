#pragma once

#include "Cpu/Common/Types.h"
#include "Cpu/Isa/Instruction.h"

#include <array>
#include <optional>
#include <vector>

namespace Cpu::Isa
{
using InstructionBytes = std::array<Byte, 4>;

InstructionBytes encodeInstruction(const Instruction &instruction);

std::optional<Instruction> decodeInstruction(InstructionBytes bytes);

Bytecode encodeProgram(const std::vector<Instruction> &instructions);

std::optional<std::vector<Instruction>> decodeProgram(const Bytecode &bytecode);

}
