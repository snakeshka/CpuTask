#include "Executor.h"

#include <stdexcept>

#include "Cpu/Isa/Opcode.h"

namespace Emulator
{
Executor::Executor()
{
	m_Handlers.fill(&Executor::ExecuteIllegal);
	m_Handlers[static_cast<std::uint8_t>(Cpu::Isa::Opcode::LI)] = &Executor::ExecuteLi;
	m_Handlers[static_cast<std::uint8_t>(Cpu::Isa::Opcode::LUI)] = &Executor::ExecuteLui;
	m_Handlers[static_cast<std::uint8_t>(Cpu::Isa::Opcode::ADD)] = &Executor::ExecuteAdd;
	m_Handlers[static_cast<std::uint8_t>(Cpu::Isa::Opcode::SUB)] = &Executor::ExecuteSub;
	m_Handlers[static_cast<std::uint8_t>(Cpu::Isa::Opcode::AND)] = &Executor::ExecuteAnd;
	m_Handlers[static_cast<std::uint8_t>(Cpu::Isa::Opcode::OR)] = &Executor::ExecuteOr;
	m_Handlers[static_cast<std::uint8_t>(Cpu::Isa::Opcode::XOR)] = &Executor::ExecuteXor;
	m_Handlers[static_cast<std::uint8_t>(Cpu::Isa::Opcode::SLL)] = &Executor::ExecuteSll;
	m_Handlers[static_cast<std::uint8_t>(Cpu::Isa::Opcode::SRL)] = &Executor::ExecuteSrl;
	m_Handlers[static_cast<std::uint8_t>(Cpu::Isa::Opcode::SRA)] = &Executor::ExecuteSra;
	m_Handlers[static_cast<std::uint8_t>(Cpu::Isa::Opcode::MUL)] = &Executor::ExecuteMul;
	m_Handlers[static_cast<std::uint8_t>(Cpu::Isa::Opcode::UDIV)] = &Executor::ExecuteUdiv;
	m_Handlers[static_cast<std::uint8_t>(Cpu::Isa::Opcode::SDIV)] = &Executor::ExecuteSdiv;
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

void Executor::ExecuteLui(CpuState &state, const Cpu::Isa::Instruction &instruction)
{
	state.SetRegister(instruction.a, (state.GetRegister(instruction.a) & 0x0000FFFF) | (static_cast<std::uint32_t>(instruction.immediate16()) << 16));
}

void Executor::ExecuteAdd(CpuState &state, const Cpu::Isa::Instruction &instruction)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) + state.GetRegister(instruction.c));
}

void Executor::ExecuteSub(CpuState &state, const Cpu::Isa::Instruction &instruction)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) - state.GetRegister(instruction.c));
}

void Executor::ExecuteAnd(CpuState &state, const Cpu::Isa::Instruction &instruction)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) & state.GetRegister(instruction.c));
}

void Executor::ExecuteOr(CpuState &state, const Cpu::Isa::Instruction &instruction)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) | state.GetRegister(instruction.c));
}

void Executor::ExecuteXor(CpuState &state, const Cpu::Isa::Instruction &instruction)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) ^ state.GetRegister(instruction.c));
}

void Executor::ExecuteSll(CpuState &state, const Cpu::Isa::Instruction &instruction)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) << (state.GetRegister(instruction.c) & 0x1F));
}

void Executor::ExecuteSrl(CpuState &state, const Cpu::Isa::Instruction &instruction)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) >> (state.GetRegister(instruction.c) & 0x1F));
}

void Executor::ExecuteSra(CpuState &state, const Cpu::Isa::Instruction &instruction)
{
	//state.SetRegister(instruction.a, static_cast<int32_t>(state.GetRegister(instruction.b)) >> (state.GetRegister(instruction.c) & 0x1F));

	uint32_t value = state.GetRegister(instruction.b);
	uint32_t shift = state.GetRegister(instruction.c) & 0x1F;

	if (shift == 0)
	{
		state.SetRegister(instruction.a, value);
		return;
	}

	uint32_t signBit = (value >> 31) & 1;
	uint32_t fillMask = (0 - signBit) << (32 - shift);
	uint32_t result = (value >> shift) | fillMask;

	state.SetRegister(instruction.a, result);
}

void Executor::ExecuteMul(CpuState &state, const Cpu::Isa::Instruction &instruction)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) * state.GetRegister(instruction.c));
}

void Executor::ExecuteUdiv(CpuState &state, const Cpu::Isa::Instruction &instruction)
{
	uint32_t divisor = state.GetRegister(instruction.c);
	if (divisor == 0)
	{
		throw std::runtime_error("division by zero in UDIV");
	}
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) / divisor);
}

void Executor::ExecuteSdiv(CpuState &state, const Cpu::Isa::Instruction &instruction)
{
	std::int32_t valB = static_cast<std::int32_t>(state.GetRegister(instruction.b));
	std::int32_t valC = static_cast<std::int32_t>(state.GetRegister(instruction.c));
	if (valC == 0)
	{
		throw std::runtime_error("division by zero in SDIV");
	}
	state.SetRegister(instruction.a, static_cast<std::uint32_t>(valB / valC));
}

}
