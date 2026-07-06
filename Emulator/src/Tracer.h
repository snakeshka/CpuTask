#pragma once

#ifndef TRACING
#define TRACING 1
#endif

#include "CpuState.h"
#include "Cpu/Isa/Instruction.h"
#include <string>

#include "Cpu/Isa/InstructionSet.h"
#include "Cpu/Isa/OperandKind.h"

namespace Emulator
{
class Tracer
{
public:
	explicit Tracer(bool enabled)
		: m_Enabled(enabled)
	{
	}

	inline void SaveStateBefore(const CpuState &state, const Cpu::Isa::Instruction &instruction)
	{
#if TRACING
		if (!m_Enabled)
			return;
		m_Addr = state.ProgramCounter();
		m_ValB = 0;
		m_ValC = 0;
		// Сверяемся с форматом команды, чтобы знать, где регистры, а где константы
		auto formatOpt = Cpu::Isa::findInstructionByOpcode(instruction.opcode);
		if (formatOpt.has_value())
		{
			// Если есть 2-й операнд и это регистр, считываем его
			if (formatOpt->operands.size() > 1 && formatOpt->operands[1] == Cpu::Isa::OperandKind::Register)
			{
				m_ValB = state.GetRegister(instruction.b);
			}
			// Если есть 3-й операнд и это регистр, считываем его
			if (formatOpt->operands.size() > 2 && formatOpt->operands[2] == Cpu::Isa::OperandKind::Register)
			{
				m_ValC = state.GetRegister(instruction.c);
			}
		}
#endif
	}

	inline void TraceStep(const CpuState &state, const Cpu::Isa::Instruction &instruction)
	{
#if TRACING
		if (!m_Enabled)
			return;
		PrintTraceLine(state, instruction);
		m_Tick++;
#endif
	}

private:
	bool m_Enabled = false;
#if TRACING
	uint32_t m_Tick = 1;
	uint32_t m_Addr = 0;
	uint32_t m_ValB = 0;
	uint32_t m_ValC = 0;

	void PrintTraceLine(const CpuState &state, const Cpu::Isa::Instruction &instruction);
#endif
};

}