#pragma once

#include "Cpu/Isa/Opcode.h"
#include "Cpu/Isa/OperandKind.h"

#include <span>
#include <string_view>

namespace Cpu::Isa
{
struct InstructionFormat
{
	std::string_view mnemonic;
	Opcode opcode;
	std::span<const OperandKind> operands;
};

}
