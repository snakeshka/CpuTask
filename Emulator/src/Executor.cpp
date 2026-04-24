#include "Executor.h"

#include "Cpu/Isa/Opcode.h"

namespace Emulator
{
Executor::Executor()
{
	m_Handlers.fill(&Executor::ExecuteIllegal);
	m_Handlers[static_cast<std::uint8_t>(Cpu::Isa::Opcode::LI)] = &Executor::ExecuteLi;
	m_Handlers[static_cast<std::uint8_t>(Cpu::Isa::Opcode::ADD)] = &Executor::ExecuteAdd;
}

void Executor::Execute(CpuState &state, const Cpu::Isa::Instruction &instruction) const
{
	const auto opcode = static_cast<std::uint8_t>(instruction.opcode);
	m_Handlers[opcode](state, instruction);
	state.AdvanceProgramCounter();
}

void Executor::ExecuteIllegal(CpuState &state, const Cpu::Isa::Instruction &instruction)
{
	(void)state;
	(void)instruction;
}

void Executor::ExecuteLi(CpuState &state, const Cpu::Isa::Instruction &instruction)
{
	state.SetRegister(instruction.a, instruction.immediate16());
}

void Executor::ExecuteAdd(CpuState &state, const Cpu::Isa::Instruction &instruction)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) + state.GetRegister(instruction.c));
}

}
