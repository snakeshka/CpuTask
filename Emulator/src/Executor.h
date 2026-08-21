#pragma once

#include <array>

#include "Cpu/Isa/Instruction.h"
#include "CpuState.h"
#include "BusRouter.h"
#include "Status.h"
#include "Tracer.h"


namespace Emulator {
struct ExecutionContext
{
	CpuState& State;
	BusRouter& Bus;
	class Tracer& Tracer;
	const Cpu::Isa::Instruction& Instruction;
	uint32_t Tick;
};

class Executor
{
public:
	using Handler = ExecutionStatus (*)(ExecutionContext&);

	Executor() = default;

	ExecutionStatus Execute(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick) const;
	ExecutionStatus Execute(ExecutionContext& ctx) const;

private:
	static consteval std::array<Handler, 256> CreateHandlersMap();

	static const std::array<Handler, 256> s_handlers;

	static ExecutionStatus ExecuteIllegal(ExecutionContext&);
	static ExecutionStatus ExecuteLi(ExecutionContext&);
	static ExecutionStatus ExecuteLui(ExecutionContext&);
	static ExecutionStatus ExecuteAdd(ExecutionContext&);
	static ExecutionStatus ExecuteSub(ExecutionContext&);
	static ExecutionStatus ExecuteAnd(ExecutionContext&);
	static ExecutionStatus ExecuteOr(ExecutionContext&);
	static ExecutionStatus ExecuteXor(ExecutionContext&);
	static ExecutionStatus ExecuteSll(ExecutionContext&);
	static ExecutionStatus ExecuteSrl(ExecutionContext&);
	static ExecutionStatus ExecuteSra(ExecutionContext&);
	static ExecutionStatus ExecuteMul(ExecutionContext&);
	static ExecutionStatus ExecuteUdiv(ExecutionContext&);
	static ExecutionStatus ExecuteSdiv(ExecutionContext&);
	static ExecutionStatus ExecuteLdb(ExecutionContext&);
	static ExecutionStatus ExecuteLdh(ExecutionContext&);
	static ExecutionStatus ExecuteLdw(ExecutionContext&);
	static ExecutionStatus ExecuteStb(ExecutionContext&);
	static ExecutionStatus ExecuteSth(ExecutionContext&);
	static ExecutionStatus ExecuteStw(ExecutionContext&);
	static ExecutionStatus ExecuteSxt(ExecutionContext&);
	static ExecutionStatus ExecuteBswap(ExecutionContext&);
	static ExecutionStatus ExecuteScrw(ExecutionContext&);
	static ExecutionStatus ExecuteScrr(ExecutionContext&);
	static ExecutionStatus ExecuteAspi(ExecutionContext&);
	static ExecutionStatus ExecuteAspr(ExecutionContext&);
	static ExecutionStatus ExecuteRjmp(ExecutionContext&);
	static ExecutionStatus ExecuteBrz(ExecutionContext&);
	static ExecutionStatus ExecuteBrnz(ExecutionContext&);
	static ExecutionStatus ExecuteAjmp(ExecutionContext&);
	static ExecutionStatus ExecuteCall(ExecutionContext&);
	static ExecutionStatus ExecuteRih(ExecutionContext&);
	static ExecutionStatus ExecuteWait(ExecutionContext&);
	static ExecutionStatus ExecuteEq(ExecutionContext&);
	static ExecutionStatus ExecuteNe(ExecutionContext&);
	static ExecutionStatus ExecuteLt(ExecutionContext&);
	static ExecutionStatus ExecuteGe(ExecutionContext&);
	static ExecutionStatus ExecuteSlt(ExecutionContext&);
	static ExecutionStatus ExecuteSge(ExecutionContext&);
	static ExecutionStatus ExecuteSimCtrl(ExecutionContext&);
};
}