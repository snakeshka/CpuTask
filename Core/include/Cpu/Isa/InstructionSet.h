#pragma once

#include "Cpu/Isa/InstructionFormat.h"

#include <optional>
#include <span>
#include <string_view>

namespace Cpu::Isa
{
std::span<const InstructionFormat> instructionSet();

std::optional<InstructionFormat> findInstructionByMnemonic(std::string_view mnemonic);

std::optional<InstructionFormat> findInstructionByOpcode(Opcode opcode);

}
