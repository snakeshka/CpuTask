#pragma once

#include "CpuState.h"
#include "Cpu/Isa/Instruction.h"

#include <array>

namespace Emulator
{
class Executor
{
public:
	Executor();

	void Execute(CpuState &state, const Cpu::Isa::Instruction &instruction) const;

private:
	using Handler = void (*)(CpuState &, const Cpu::Isa::Instruction &);

	std::array<Handler, 256> m_Handlers{};

	static void ExecuteIllegal(CpuState &state, const Cpu::Isa::Instruction &instruction);
	static void ExecuteLi(CpuState &state, const Cpu::Isa::Instruction &instruction);
	static void ExecuteLui(CpuState &state, const Cpu::Isa::Instruction &instruction);
	static void ExecuteAdd(CpuState &state, const Cpu::Isa::Instruction &instruction);
	static void ExecuteSub(CpuState &state, const Cpu::Isa::Instruction &instruction);
	static void ExecuteAnd(CpuState &state, const Cpu::Isa::Instruction &instruction);
	static void ExecuteOr(CpuState &state, const Cpu::Isa::Instruction &instruction);
	static void ExecuteXor(CpuState &state, const Cpu::Isa::Instruction &instruction);
	static void ExecuteSll(CpuState &state, const Cpu::Isa::Instruction &instruction);
	static void ExecuteSrl(CpuState &state, const Cpu::Isa::Instruction &instruction);
	static void ExecuteSra(CpuState &state, const Cpu::Isa::Instruction &instruction);
	static void ExecuteMul(CpuState &state, const Cpu::Isa::Instruction &instruction);
	static void ExecuteUdiv(CpuState &state, const Cpu::Isa::Instruction &instruction);
	static void ExecuteSdiv(CpuState &state, const Cpu::Isa::Instruction &instruction);
};

}
