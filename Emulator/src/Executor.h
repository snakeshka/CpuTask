#pragma once

#include "CpuState.h"
#include "Ram.h"
#include "Cpu/Isa/Instruction.h"

#include <array>

#include "Status.h"


namespace Emulator {
class Executor
{
public:
	Executor();

	ExecutionStatus Execute(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick) const;

private:
	using Handler = ExecutionStatus (*)(CpuState&, Ram&, const Cpu::Isa::Instruction&, uint32_t);

	static constexpr std::array<Handler, 256> CreateHandlersMap();

	std::array<Handler, 256> m_Handlers{};

	static ExecutionStatus ExecuteIllegal(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteLi(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteLui(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteAdd(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteSub(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteAnd(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteOr(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteXor(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteSll(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteSrl(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteSra(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteMul(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteUdiv(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteSdiv(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteLdb(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteLdh(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteLdw(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteStb(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteSth(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteStw(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteSxt(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteBswap(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteScrw(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteScrr(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteAspi(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteAspr(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteRjmp(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteBrz(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteBrnz(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteAjmp(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteCall(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteRih(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteEq(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteNe(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteLt(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteGe(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteSlt(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteSge(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick);
};
}