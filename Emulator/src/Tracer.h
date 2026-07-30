#pragma once

#ifndef TRACING
#define TRACING 1
#endif

#include "CpuState.h"
#include "Cpu/Isa/Instruction.h"
#include <string>
#include <algorithm>

#include "Cpu/Isa/InstructionSet.h"
#include "Cpu/Isa/OperandKind.h"

namespace Emulator {
struct TickInterval
{
	uint32_t begin;
	uint32_t end;
};

class Tracer
{
public:
	explicit Tracer(const bool enabled, std::vector<TickInterval> intervals = {}, bool traceIntEnabled = false)
		: m_Enabled(enabled)
		  , m_Intervals(std::move(intervals))
		  , m_TraceIntEnabled(traceIntEnabled)
	{
	}

	inline void SaveStateBefore(const CpuState& state, const Cpu::Isa::Instruction& instruction)
	{
#if TRACING
		if (!m_Enabled)
			return;
		if (!IsTickAllowed(m_Tick))
			return;

		m_Addr = state.InstructionPointer();
		m_ValB = 0;
		m_ValC = 0;

		auto formatOpt = Cpu::Isa::findInstructionByOpcode(instruction.opcode);
		if (formatOpt.has_value()) {
			if (formatOpt->operands.size() > 1 && formatOpt->operands[1] == Cpu::Isa::OperandKind::Register) {
				m_ValB = state.GetRegister(instruction.b);
			}
			if (formatOpt->operands.size() > 2 && formatOpt->operands[2] == Cpu::Isa::OperandKind::Register) {
				m_ValC = state.GetRegister(instruction.c);
			}
		}
#endif
	}

	inline void TraceStep(const CpuState& state, const Cpu::Isa::Instruction& instruction)
	{
#if TRACING
		if (!m_Enabled)
			return;
		if (IsTickAllowed(m_Tick)) {
			PrintTraceLine(state, instruction);
		}
		m_Tick++;
#endif
	}

	void TraceInterrupt(uint32_t tick, uint32_t iip, uint32_t icr, uint32_t ihr);
	void PrintTraceLine(const CpuState& state, const Cpu::Isa::Instruction& instruction, bool interrupted = false) const;

private:
	bool m_Enabled = false;
#if TRACING
	uint32_t m_Tick = 1;
	uint32_t m_Addr = 0;
	uint32_t m_ValB = 0;
	uint32_t m_ValC = 0;
	std::vector<TickInterval> m_Intervals;
	bool m_TraceIntEnabled = false;

	inline bool IsTickAllowed(uint32_t tick) const
	{
		if (m_Intervals.empty())
			return true;
		auto it = std::lower_bound(m_Intervals.begin(),
		                           m_Intervals.end(),
		                           tick,
		                           [](const TickInterval& interval, uint32_t val) {
			                           return interval.end < val;
		                           });
		if (it != m_Intervals.end() && tick >= it->begin) {
			return true;
		}
		return false;
	}

#endif
};
}