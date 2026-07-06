#pragma once

#include "CpuState.h"
#include "Cpu/Isa/Instruction.h"

#include <array>

#include "Status.h"

namespace Emulator
{
class Executor
{
public:
	Executor();

	ExecutionStatus Execute(CpuState &state, const Cpu::Isa::Instruction &instruction) const;

private:
	using Handler = ExecutionStatus (*)(CpuState &, const Cpu::Isa::Instruction &);

	std::array<Handler, 256> m_Handlers{};

	static ExecutionStatus ExecuteIllegal(CpuState &state, const Cpu::Isa::Instruction &instruction);
	static ExecutionStatus ExecuteLi(CpuState &state, const Cpu::Isa::Instruction &instruction);
	static ExecutionStatus ExecuteLui(CpuState &state, const Cpu::Isa::Instruction &instruction);
	static ExecutionStatus ExecuteAdd(CpuState &state, const Cpu::Isa::Instruction &instruction);
	static ExecutionStatus ExecuteSub(CpuState &state, const Cpu::Isa::Instruction &instruction);
	static ExecutionStatus ExecuteAnd(CpuState &state, const Cpu::Isa::Instruction &instruction);
	static ExecutionStatus ExecuteOr(CpuState &state, const Cpu::Isa::Instruction &instruction);
	static ExecutionStatus ExecuteXor(CpuState &state, const Cpu::Isa::Instruction &instruction);
	static ExecutionStatus ExecuteSll(CpuState &state, const Cpu::Isa::Instruction &instruction);
	static ExecutionStatus ExecuteSrl(CpuState &state, const Cpu::Isa::Instruction &instruction);
	static ExecutionStatus ExecuteSra(CpuState &state, const Cpu::Isa::Instruction &instruction);
	static ExecutionStatus ExecuteMul(CpuState &state, const Cpu::Isa::Instruction &instruction);
	static ExecutionStatus ExecuteUdiv(CpuState &state, const Cpu::Isa::Instruction &instruction);
	static ExecutionStatus ExecuteSdiv(CpuState &state, const Cpu::Isa::Instruction &instruction);
};

}
