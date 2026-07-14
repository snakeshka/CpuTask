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

ExecutionStatus Executor::Execute(CpuState &state, Ram &ram, const Cpu::Isa::Instruction &instruction, uint32_t tick) const
{
	const auto opcode = static_cast<uint8_t>(instruction.opcode);
	auto status = m_Handlers[opcode](state, ram, instruction, tick);

	if (status == ExecutionStatus::Success)
	{
		state.AdvanceInstructionPointer();
	}

	return status;
}

ExecutionStatus Executor::ExecuteIllegal(CpuState &state, Ram &ram, const Cpu::Isa::Instruction &instruction, uint32_t tick)
{
	(void)state;
	(void)instruction;
	return ExecutionStatus::IllegalInstruction;
}

ExecutionStatus Executor::ExecuteLi(CpuState &state, Ram &ram, const Cpu::Isa::Instruction &instruction, uint32_t tick)
{
	state.SetRegister(instruction.a, instruction.immediate16());
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteLui(CpuState &state, Ram &ram, const Cpu::Isa::Instruction &instruction, uint32_t tick)
{
	state.SetRegister(instruction.a, (state.GetRegister(instruction.a) & 0x0000FFFF) | (static_cast<uint32_t>(instruction.immediate16()) << 16));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteAdd(CpuState &state, Ram &ram, const Cpu::Isa::Instruction &instruction, uint32_t tick)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) + state.GetRegister(instruction.c));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteSub(CpuState &state, Ram &ram, const Cpu::Isa::Instruction &instruction, uint32_t tick)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) - state.GetRegister(instruction.c));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteAnd(CpuState &state, Ram &ram, const Cpu::Isa::Instruction &instruction, uint32_t tick)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) & state.GetRegister(instruction.c));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteOr(CpuState &state, Ram &ram, const Cpu::Isa::Instruction &instruction, uint32_t tick)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) | state.GetRegister(instruction.c));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteXor(CpuState &state, Ram &ram, const Cpu::Isa::Instruction &instruction, uint32_t tick)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) ^ state.GetRegister(instruction.c));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteSll(CpuState &state, Ram &ram, const Cpu::Isa::Instruction &instruction, uint32_t tick)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) << (state.GetRegister(instruction.c) & 0x1F));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteSrl(CpuState &state, Ram &ram, const Cpu::Isa::Instruction &instruction, uint32_t tick)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) >> (state.GetRegister(instruction.c) & 0x1F));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteSra(CpuState &state, Ram &ram, const Cpu::Isa::Instruction &instruction, uint32_t tick)
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

ExecutionStatus Executor::ExecuteMul(CpuState &state, Ram &ram, const Cpu::Isa::Instruction &instruction, uint32_t tick)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) * state.GetRegister(instruction.c));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteUdiv(CpuState &state, Ram &ram, const Cpu::Isa::Instruction &instruction, uint32_t tick)
{
	uint32_t divisor = state.GetRegister(instruction.c);
	if (divisor == 0)
	{
		return ExecutionStatus::DivisionByZero;
	}
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) / divisor);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteSdiv(CpuState &state, Ram &ram, const Cpu::Isa::Instruction &instruction, uint32_t tick)
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

ExecutionStatus Executor::ExecuteLdb(CpuState &state, Ram &ram, const Cpu::Isa::Instruction &instruction, uint32_t tick)
{
	uint32_t addr = state.GetRegister(instruction.b) + instruction.immediate16();
	if (!ram.IsValidRange(addr, 1))
	{
		return ExecutionStatus::MemoryOutOfBounds;
	}
	state.SetRegister(instruction.a, ram.ReadByte(addr, tick));
	return ExecutionStatus::Success;
}
ExecutionStatus Executor::ExecuteLdh(CpuState &state, Ram &ram, const Cpu::Isa::Instruction &instruction, uint32_t tick)
{
	uint32_t addr = state.GetRegister(instruction.b) + instruction.immediate16();
	if (!ram.IsValidRange(addr, 2))
	{
		return ExecutionStatus::MemoryOutOfBounds;
	}
	state.SetRegister(instruction.a, ram.ReadHalfword(addr, tick));
	return ExecutionStatus::Success;
}
ExecutionStatus Executor::ExecuteLdw(CpuState &state, Ram &ram, const Cpu::Isa::Instruction &instruction, uint32_t tick)
{
	uint32_t addr = state.GetRegister(instruction.b) + instruction.immediate16();
	if (!ram.IsValidRange(addr, 4))
	{
		return ExecutionStatus::MemoryOutOfBounds;
	}
	state.SetRegister(instruction.a, ram.ReadWord(addr, tick));
	return ExecutionStatus::Success;
}
ExecutionStatus Executor::ExecuteStb(CpuState &state, Ram &ram, const Cpu::Isa::Instruction &instruction, uint32_t tick)
{
	uint32_t addr = state.GetRegister(instruction.b) + instruction.immediate16();
	if (!ram.IsValidRange(addr, 1))
	{
		return ExecutionStatus::MemoryOutOfBounds;
	}
	ram.WriteByte(addr, static_cast<uint8_t>(state.GetRegister(instruction.a) & 0xFF), tick);
	return ExecutionStatus::Success;
}
ExecutionStatus Executor::ExecuteSth(CpuState &state, Ram &ram, const Cpu::Isa::Instruction &instruction, uint32_t tick)
{
	uint32_t addr = state.GetRegister(instruction.b) + instruction.immediate16();
	if (!ram.IsValidRange(addr, 2))
	{
		return ExecutionStatus::MemoryOutOfBounds;
	}
	ram.WriteHalfword(addr, static_cast<uint16_t>(state.GetRegister(instruction.a) & 0xFFFF), tick);
	return ExecutionStatus::Success;
}
ExecutionStatus Executor::ExecuteStw(CpuState &state, Ram &ram, const Cpu::Isa::Instruction &instruction, uint32_t tick)
{
	uint32_t addr = state.GetRegister(instruction.b) + instruction.immediate16();
	if (!ram.IsValidRange(addr, 4))
	{
		return ExecutionStatus::MemoryOutOfBounds;
	}
	ram.WriteWord(addr, state.GetRegister(instruction.a), tick);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteSxt(CpuState &state, Ram &ram, const Cpu::Isa::Instruction &instruction, uint32_t tick)
{
	uint32_t val = state.GetRegister(instruction.b);
	uint32_t imm = instruction.immediate16();
	uint32_t res = val;
	if (imm == 0)
	{
		uint32_t sign_bit = (val >> 7) & 1;
		uint32_t mask = (0 - sign_bit) << 8; // 0xFFFFFF00 если знаковый бит равен 1, иначе 0x00000000
		res = (val & 0xFF) | mask;
	}
	else if (imm == 1)
	{
		uint32_t sign_bit = (val >> 15) & 1;
		uint32_t mask = (0 - sign_bit) << 16;
		res = (val & 0xFFFF) | mask;
	}

	state.SetRegister(instruction.a, res);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteBswap(CpuState &state, Ram &ram, const Cpu::Isa::Instruction &instruction, uint32_t tick)
{
	uint32_t val = state.GetRegister(instruction.b);
	uint32_t imm = instruction.immediate16();
	uint32_t res = val;
	if (imm == 1)
	{
		uint32_t low = val & 0xFF;
		uint32_t high = (val >> 8) & 0xFF;
		res = (val & 0xFFFF0000) | (low << 8) | high;
	}
	else if (imm == 2)
	{
		uint32_t b0 = val & 0xFF;
		uint32_t b1 = (val >> 8) & 0xFF;
		uint32_t b2 = (val >> 16) & 0xFF;
		uint32_t b3 = (val >> 24) & 0xFF;
		res = (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;
	}
	state.SetRegister(instruction.a, res);
	return ExecutionStatus::Success;
}

}
