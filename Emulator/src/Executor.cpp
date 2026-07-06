#include "Executor.h"

#include <stdexcept>

#include "Cpu/Isa/Opcode.h"

namespace Emulator
{
Executor::Executor()
{
	m_Handlers.fill(&Executor::ExecuteIllegal);
	m_Handlers[static_cast<uint8_t>(Cpu::Isa::Opcode::LI)] = &Executor::ExecuteLi;
	m_Handlers[static_cast<uint8_t>(Cpu::Isa::Opcode::LUI)] = &Executor::ExecuteLui;
	m_Handlers[static_cast<uint8_t>(Cpu::Isa::Opcode::ADD)] = &Executor::ExecuteAdd;
	m_Handlers[static_cast<uint8_t>(Cpu::Isa::Opcode::SUB)] = &Executor::ExecuteSub;
	m_Handlers[static_cast<uint8_t>(Cpu::Isa::Opcode::AND)] = &Executor::ExecuteAnd;
	m_Handlers[static_cast<uint8_t>(Cpu::Isa::Opcode::OR)] = &Executor::ExecuteOr;
	m_Handlers[static_cast<uint8_t>(Cpu::Isa::Opcode::XOR)] = &Executor::ExecuteXor;
	m_Handlers[static_cast<uint8_t>(Cpu::Isa::Opcode::SLL)] = &Executor::ExecuteSll;
	m_Handlers[static_cast<uint8_t>(Cpu::Isa::Opcode::SRL)] = &Executor::ExecuteSrl;
	m_Handlers[static_cast<uint8_t>(Cpu::Isa::Opcode::SRA)] = &Executor::ExecuteSra;
	m_Handlers[static_cast<uint8_t>(Cpu::Isa::Opcode::MUL)] = &Executor::ExecuteMul;
	m_Handlers[static_cast<uint8_t>(Cpu::Isa::Opcode::UDIV)] = &Executor::ExecuteUdiv;
	m_Handlers[static_cast<uint8_t>(Cpu::Isa::Opcode::SDIV)] = &Executor::ExecuteSdiv;
}

ExecutionStatus Executor::Execute(CpuState &state, const Cpu::Isa::Instruction &instruction) const
{
	const auto opcode = static_cast<uint8_t>(instruction.opcode);
	auto status = m_Handlers[opcode](state, instruction);

	if (status == ExecutionStatus::Success)
	{
		state.AdvanceProgramCounter();
	}

	return status;
}

ExecutionStatus Executor::ExecuteIllegal(CpuState &state, const Cpu::Isa::Instruction &instruction)
{
	(void)state;
	(void)instruction;
	return ExecutionStatus::IllegalInstruction;
}

ExecutionStatus Executor::ExecuteLi(CpuState &state, const Cpu::Isa::Instruction &instruction)
{
	state.SetRegister(instruction.a, instruction.immediate16());
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteLui(CpuState &state, const Cpu::Isa::Instruction &instruction)
{
	state.SetRegister(instruction.a, (state.GetRegister(instruction.a) & 0x0000FFFF) | (static_cast<uint32_t>(instruction.immediate16()) << 16));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteAdd(CpuState &state, const Cpu::Isa::Instruction &instruction)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) + state.GetRegister(instruction.c));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteSub(CpuState &state, const Cpu::Isa::Instruction &instruction)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) - state.GetRegister(instruction.c));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteAnd(CpuState &state, const Cpu::Isa::Instruction &instruction)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) & state.GetRegister(instruction.c));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteOr(CpuState &state, const Cpu::Isa::Instruction &instruction)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) | state.GetRegister(instruction.c));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteXor(CpuState &state, const Cpu::Isa::Instruction &instruction)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) ^ state.GetRegister(instruction.c));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteSll(CpuState &state, const Cpu::Isa::Instruction &instruction)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) << (state.GetRegister(instruction.c) & 0x1F));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteSrl(CpuState &state, const Cpu::Isa::Instruction &instruction)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) >> (state.GetRegister(instruction.c) & 0x1F));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteSra(CpuState &state, const Cpu::Isa::Instruction &instruction)
{
	//state.SetRegister(instruction.a, static_cast<int32_t>(state.GetRegister(instruction.b)) >> (state.GetRegister(instruction.c) & 0x1F));

	uint32_t value = state.GetRegister(instruction.b);
	uint32_t shift = state.GetRegister(instruction.c) & 0x1F;

	if (shift == 0)
	{
		state.SetRegister(instruction.a, value);
		return ExecutionStatus::Success;
	}

	uint32_t signBit = (value >> 31) & 1;
	uint32_t fillMask = (0 - signBit) << (32 - shift);
	uint32_t result = (value >> shift) | fillMask;

	state.SetRegister(instruction.a, result);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteMul(CpuState &state, const Cpu::Isa::Instruction &instruction)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) * state.GetRegister(instruction.c));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteUdiv(CpuState &state, const Cpu::Isa::Instruction &instruction)
{
	uint32_t divisor = state.GetRegister(instruction.c);
	if (divisor == 0)
	{
		return ExecutionStatus::DivisionByZero;
	}
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) / divisor);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteSdiv(CpuState &state, const Cpu::Isa::Instruction &instruction)
{
	int32_t valB = static_cast<int32_t>(state.GetRegister(instruction.b));
	int32_t valC = static_cast<int32_t>(state.GetRegister(instruction.c));
	if (valC == 0)
	{
		return ExecutionStatus::DivisionByZero;
	}
	state.SetRegister(instruction.a, static_cast<uint32_t>(valB / valC));
	return ExecutionStatus::Success;
}

}
