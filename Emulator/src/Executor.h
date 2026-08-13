#pragma once

#include <array>

#include "Cpu/Isa/Instruction.h"
#include "CpuState.h"
#include "BusRouter.h"
#include "Status.h"
#include "Tracer.h"


namespace Emulator {
class Executor
{
public:
	Executor();

	ExecutionStatus Execute(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick) const;

private:
	using Handler = ExecutionStatus (*)(CpuState&, BusRouter&, Tracer&, const Cpu::Isa::Instruction&, uint32_t);

	static constexpr std::array<Handler, 256> CreateHandlersMap();

	std::array<Handler, 256> m_Handlers{};

	static ExecutionStatus ExecuteIllegal(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteLi(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteLui(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteAdd(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteSub(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteAnd(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteOr(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteXor(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteSll(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteSrl(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteSra(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteMul(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteUdiv(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteSdiv(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteLdb(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteLdh(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteLdw(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteStb(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteSth(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteStw(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteSxt(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteBswap(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteScrw(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteScrr(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteAspi(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteAspr(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteRjmp(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteBrz(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteBrnz(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteAjmp(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteCall(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteRih(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteEq(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteNe(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteLt(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteGe(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteSlt(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteSge(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
	static ExecutionStatus ExecuteSimCtrl(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick);
};
}