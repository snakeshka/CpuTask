#pragma once

#include <cstdint>

namespace Cpu::Isa
{
enum class OperandKind : uint8_t
{
	Register,
	SystemRegister,
	Immediate16,
	UpperImmediate16
};

}