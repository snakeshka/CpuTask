#pragma once

#include <cstdint>

namespace Cpu::Isa {

enum class Opcode : std::uint8_t {
	LI = 0x00,
	LUI = 0x01,
	ADD = 0x02,
	SUB = 0x03,
	AND = 0X04,
	OR = 0X05,
	XOR = 0X06,
	SLL = 0X07,
	SRL = 0X08,
	SRA = 0X09,
	MUL = 0X0A,
	UDIV = 0X0B,
	SDIV = 0X0C
};

}
