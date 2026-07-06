#include "Tracer.h"
#include "Cpu/Isa/InstructionSet.h"
#include <iostream>
#include <iomanip>

#if TRACING
namespace Emulator
{
void Tracer::PrintTraceLine(const CpuState &state, const Cpu::Isa::Instruction &instruction)
{
	auto formatOpt = Cpu::Isa::findInstructionByOpcode(instruction.opcode);
	if (!formatOpt.has_value())
		return;

	std::cout << "---- " << std::dec << m_Tick << " 0x" << std::hex << m_Addr << " ----\n";

	std::uint32_t encoding = static_cast<std::uint8_t>(instruction.opcode) |
		(static_cast<std::uint32_t>(instruction.a) << 8) |
		(static_cast<std::uint32_t>(instruction.b) << 16) |
		(static_cast<std::uint32_t>(instruction.c) << 24);

	std::cout << "0x" << std::setfill('0') << std::setw(8) << std::hex << encoding << " " << formatOpt->mnemonic;

	for (std::size_t i = 0; i < formatOpt->operands.size(); ++i)
	{
		auto kind = formatOpt->operands[i];
		if (kind == Cpu::Isa::OperandKind::Register)
		{
			if (i == 0)
			{
				std::cout << " R" << std::dec << static_cast<int>(instruction.a)
						  << " (0x" << std::hex << state.GetRegister(instruction.a) << ")";
			}
			else if (i == 1)
			{
				std::cout << " R" << std::dec << static_cast<int>(instruction.b)
						  << " (0x" << std::hex << m_ValB << ")";
			}
			else if (i == 2)
			{
				std::cout << " R" << std::dec << static_cast<int>(instruction.c)
						  << " (0x" << std::hex << m_ValC << ")";
			}
		}
		else if (kind == Cpu::Isa::OperandKind::Immediate16 || kind == Cpu::Isa::OperandKind::UpperImmediate16)
		{
			std::cout << " imm (0x" << std::hex << instruction.immediate16() << ")";
		}
	}
	std::cout << "\n";
}

}
#endif