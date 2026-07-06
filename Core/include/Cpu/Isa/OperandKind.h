#pragma once

namespace Cpu::Isa
{
enum class OperandKind
{
	Register,
	Immediate16,
	UpperImmediate16
};

}