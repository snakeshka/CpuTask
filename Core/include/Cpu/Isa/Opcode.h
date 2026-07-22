#pragma once

#include <cstdint>

namespace Cpu::Isa {

enum class Opcode : uint8_t {
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
	SDIV = 0X0C,
	LDB = 0x10,
	LDH = 0x11,
	LDW = 0x12,
	STB = 0x13,
	STH = 0x14,
	STW = 0x15,
	SXT = 0x16,
	BSWAP = 0x17,
	SCRW = 0x20,
	SCRR = 0x21,
	ASPI = 0x22,
	ASPR = 0x23,
	RJMP = 0x30,
	BRZ = 0x31,
	BRNZ = 0x32,
	AJMP = 0x33,
	CALL = 0x34,
	EQ = 0x40,
	NE = 0x41,
	LT = 0x42,
	GE = 0x43,
	SLT = 0x44,
	SGE = 0x45
};

}
