#pragma once

#include "Cpu/Isa/Opcode.h"
#include "Cpu/Common/Types.h"

namespace Cpu::Isa
{

struct Instruction
{
	Opcode opcode{};
	Cpu::Byte a{};
	Cpu::Byte b{};
	Cpu::Byte c{};

	Cpu::Word immediate16() const
	{
		return static_cast<Cpu::Word>(b) |
				(static_cast<Cpu::Word>(c) << 8);
	}
};

}