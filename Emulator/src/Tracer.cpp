#include "Tracer.h"
#include "Cpu/Isa/InstructionSet.h"
#include <iostream>
#include <iomanip>

#if TRACING
namespace Emulator {
void Tracer::TraceInterrupt(uint32_t tick, uint32_t iip, uint32_t icr, uint32_t ihr)
{
	if (m_TraceIntEnabled) {
		std::cout << std::dec << tick << " INT IIP=0x" << std::hex << iip
			<< " ICR=" << std::dec << icr
			<< " IP=0x" << std::hex << ihr << "\n";
	}
}

void Tracer::PrintTraceLine(const CpuState& state, const Cpu::Isa::Instruction& instruction, bool interrupted) const
{
	auto formatOpt = Cpu::Isa::findInstructionByOpcode(instruction.opcode);
	if (!formatOpt.has_value())
		return;

	std::cout << "---- " << std::dec << m_Tick << " 0x" << std::hex << m_Addr;
	if (interrupted) {
		std::cout << " (interrupted)";
	}
	std::cout << " ----\n";

	uint32_t encoding = static_cast<uint8_t>(instruction.opcode) |
		(static_cast<uint32_t>(instruction.a) << 8) |
		(static_cast<uint32_t>(instruction.b) << 16) |
		(static_cast<uint32_t>(instruction.c) << 24);

	std::cout << "0x" << std::setfill('0') << std::setw(8) << std::hex << encoding;

	if (instruction.opcode == Cpu::Isa::Opcode::SIM_CTRL) {
		if (instruction.a == 0xFF) {
			if (instruction.b == 0x01) {
				std::cout << " _SIM_GET_TICK R" << std::dec << static_cast<int>(instruction.c)
					<< " (0x" << std::hex << state.GetRegister(instruction.c) << ")";
			} else if (instruction.b == 0x02) {
				std::cout << " _SIM_TRACE_DISASM R" << std::dec << static_cast<int>(instruction.c)
					<< " (0x" << std::hex << state.GetRegister(instruction.c) << ")";
			} else if (instruction.b == 0x00) {
				std::cout << " _SIM_HALTR R" << std::dec << static_cast<int>(instruction.c)
					<< " (0x" << std::hex << state.GetRegister(instruction.c) << ")";
			}
		} else if (instruction.a == 0x00) {
			std::cout << " _SIM_HALTI imm (0x" << std::hex << instruction.immediate16() << ")";
		}
	} else {
		std::cout << " " << formatOpt->mnemonic;
		for (size_t i = 0; i < formatOpt->operands.size(); ++i) {
			auto kind = formatOpt->operands[i];
			if (kind == Cpu::Isa::OperandKind::Register) {
				if (i == 0) {
					std::cout << " R" << std::dec << static_cast<int>(instruction.a)
						<< " (0x" << std::hex << state.GetRegister(instruction.a) << ")";
				} else if (i == 1) {
					std::cout << " R" << std::dec << static_cast<int>(instruction.b)
						<< " (0x" << std::hex << m_ValB << ")";
				} else if (i == 2) {
					std::cout << " R" << std::dec << static_cast<int>(instruction.c)
						<< " (0x" << std::hex << m_ValC << ")";
				}
			} else if (kind == Cpu::Isa::OperandKind::Immediate16 || kind == Cpu::Isa::OperandKind::UpperImmediate16) {
				std::cout << " imm (0x" << std::hex << instruction.immediate16() << ")";
			}
		}
	}
	std::cout << "\n";
}
}
#endif