#include "Executor.h"

#include <stdexcept>

#include "Cpu/Isa/Opcode.h"

namespace Emulator {
Executor::Executor()
	: m_Handlers(CreateHandlersMap())
{
}

namespace {
	constexpr uint8_t toIdx(Cpu::Isa::Opcode op)
	{
		return static_cast<uint8_t>(op);
	}
}

ExecutionStatus Executor::Execute(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick) const
{
	const auto opcode = static_cast<uint8_t>(instruction.opcode);
	CPU_ASSERT(opcode < m_Handlers.size(), "Opcode value exceeds handlers array boundary!");

	auto status = m_Handlers[opcode](state, ram, instruction, tick);

	if (status == ExecutionStatus::Success) {
		state.AdvanceInstructionPointer();
	}

	return status;
}

constexpr std::array<Executor::Handler, 256> Executor::CreateHandlersMap()
{
	std::array<Handler, 256> handlers{};

	handlers.fill(&Executor::ExecuteIllegal);
	handlers[toIdx(Cpu::Isa::Opcode::LI)] = &Executor::ExecuteLi;
	handlers[toIdx(Cpu::Isa::Opcode::LUI)] = &Executor::ExecuteLui;
	handlers[toIdx(Cpu::Isa::Opcode::ADD)] = &Executor::ExecuteAdd;
	handlers[toIdx(Cpu::Isa::Opcode::SUB)] = &Executor::ExecuteSub;
	handlers[toIdx(Cpu::Isa::Opcode::AND)] = &Executor::ExecuteAnd;
	handlers[toIdx(Cpu::Isa::Opcode::OR)] = &Executor::ExecuteOr;
	handlers[toIdx(Cpu::Isa::Opcode::XOR)] = &Executor::ExecuteXor;
	handlers[toIdx(Cpu::Isa::Opcode::SLL)] = &Executor::ExecuteSll;
	handlers[toIdx(Cpu::Isa::Opcode::SRL)] = &Executor::ExecuteSrl;
	handlers[toIdx(Cpu::Isa::Opcode::SRA)] = &Executor::ExecuteSra;
	handlers[toIdx(Cpu::Isa::Opcode::MUL)] = &Executor::ExecuteMul;
	handlers[toIdx(Cpu::Isa::Opcode::UDIV)] = &Executor::ExecuteUdiv;
	handlers[toIdx(Cpu::Isa::Opcode::SDIV)] = &Executor::ExecuteSdiv;
	handlers[toIdx(Cpu::Isa::Opcode::LDB)] = &Executor::ExecuteLdb;
	handlers[toIdx(Cpu::Isa::Opcode::LDH)] = &Executor::ExecuteLdh;
	handlers[toIdx(Cpu::Isa::Opcode::LDW)] = &Executor::ExecuteLdw;
	handlers[toIdx(Cpu::Isa::Opcode::STB)] = &Executor::ExecuteStb;
	handlers[toIdx(Cpu::Isa::Opcode::STH)] = &Executor::ExecuteSth;
	handlers[toIdx(Cpu::Isa::Opcode::STW)] = &Executor::ExecuteStw;
	handlers[toIdx(Cpu::Isa::Opcode::SXT)] = &Executor::ExecuteSxt;
	handlers[toIdx(Cpu::Isa::Opcode::BSWAP)] = &Executor::ExecuteBswap;
	handlers[toIdx(Cpu::Isa::Opcode::SCRW)] = &Executor::ExecuteScrw;
	handlers[toIdx(Cpu::Isa::Opcode::SCRR)] = &Executor::ExecuteScrr;
	handlers[toIdx(Cpu::Isa::Opcode::ASPI)] = &Executor::ExecuteAspi;
	handlers[toIdx(Cpu::Isa::Opcode::ASPR)] = &Executor::ExecuteAspr;
	handlers[toIdx(Cpu::Isa::Opcode::RJMP)] = &Executor::ExecuteRjmp;
	handlers[toIdx(Cpu::Isa::Opcode::BRZ)] = &Executor::ExecuteBrz;
	handlers[toIdx(Cpu::Isa::Opcode::BRNZ)] = &Executor::ExecuteBrnz;
	handlers[toIdx(Cpu::Isa::Opcode::AJMP)] = &Executor::ExecuteAjmp;
	handlers[toIdx(Cpu::Isa::Opcode::CALL)] = &Executor::ExecuteCall;
	handlers[toIdx(Cpu::Isa::Opcode::EQ)] = &Executor::ExecuteEq;
	handlers[toIdx(Cpu::Isa::Opcode::NE)] = &Executor::ExecuteNe;
	handlers[toIdx(Cpu::Isa::Opcode::LT)] = &Executor::ExecuteLt;
	handlers[toIdx(Cpu::Isa::Opcode::GE)] = &Executor::ExecuteGe;
	handlers[toIdx(Cpu::Isa::Opcode::SLT)] = &Executor::ExecuteSlt;
	handlers[toIdx(Cpu::Isa::Opcode::SGE)] = &Executor::ExecuteSge;

	return handlers;
}

ExecutionStatus Executor::ExecuteIllegal(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	(void)state;
	(void)instruction;
	return ExecutionStatus::IllegalInstruction;
}

ExecutionStatus Executor::ExecuteLi(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	state.SetRegister(instruction.a, instruction.immediate16());
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteLui(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	state.SetRegister(instruction.a, (state.GetRegister(instruction.a) & 0x0000FFFF) | (static_cast<uint32_t>(instruction.immediate16()) << 16));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteAdd(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) + state.GetRegister(instruction.c));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteSub(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) - state.GetRegister(instruction.c));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteAnd(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) & state.GetRegister(instruction.c));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteOr(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) | state.GetRegister(instruction.c));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteXor(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) ^ state.GetRegister(instruction.c));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteSll(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) << (state.GetRegister(instruction.c) & 0x1F));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteSrl(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) >> (state.GetRegister(instruction.c) & 0x1F));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteSra(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	//state.SetRegister(instruction.a, static_cast<int32_t>(state.GetRegister(instruction.b)) >> (state.GetRegister(instruction.c) & 0x1F));

	uint32_t value = state.GetRegister(instruction.b);
	uint32_t shift = state.GetRegister(instruction.c) & 0x1F;

	if (shift == 0) {
		state.SetRegister(instruction.a, value);
		return ExecutionStatus::Success;
	}

	uint32_t signBit = (value >> 31) & 1;
	uint32_t fillMask = (0 - signBit) << (32 - shift);
	uint32_t result = (value >> shift) | fillMask;

	state.SetRegister(instruction.a, result);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteMul(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) * state.GetRegister(instruction.c));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteUdiv(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	uint32_t divisor = state.GetRegister(instruction.c);
	if (divisor == 0) {
		return ExecutionStatus::DivisionByZero;
	}
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) / divisor);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteSdiv(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	int32_t valB = static_cast<int32_t>(state.GetRegister(instruction.b));
	int32_t valC = static_cast<int32_t>(state.GetRegister(instruction.c));
	if (valC == 0) {
		return ExecutionStatus::DivisionByZero;
	}
	state.SetRegister(instruction.a, static_cast<uint32_t>(valB / valC));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteLdb(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	uint32_t addr = state.GetRegister(instruction.b) + instruction.immediate16();
	if (!ram.IsValidRange(addr, 1)) {
		return ExecutionStatus::MemoryOutOfBounds;
	}
	state.SetRegister(instruction.a, ram.ReadByte(addr, tick));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteLdh(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	uint32_t addr = state.GetRegister(instruction.b) + instruction.immediate16();
	if (!ram.IsValidRange(addr, 2)) {
		return ExecutionStatus::MemoryOutOfBounds;
	}
	state.SetRegister(instruction.a, ram.ReadHalfword(addr, tick));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteLdw(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	uint32_t addr = state.GetRegister(instruction.b) + instruction.immediate16();
	if (!ram.IsValidRange(addr, 4)) {
		return ExecutionStatus::MemoryOutOfBounds;
	}
	state.SetRegister(instruction.a, ram.ReadWord(addr, tick));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteStb(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	uint32_t addr = state.GetRegister(instruction.b) + instruction.immediate16();
	if (!ram.IsValidRange(addr, 1)) {
		return ExecutionStatus::MemoryOutOfBounds;
	}
	ram.WriteByte(addr, static_cast<uint8_t>(state.GetRegister(instruction.a) & 0xFF), tick);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteSth(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	uint32_t addr = state.GetRegister(instruction.b) + instruction.immediate16();
	if (!ram.IsValidRange(addr, 2)) {
		return ExecutionStatus::MemoryOutOfBounds;
	}
	ram.WriteHalfword(addr, static_cast<uint16_t>(state.GetRegister(instruction.a) & 0xFFFF), tick);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteStw(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	uint32_t addr = state.GetRegister(instruction.b) + instruction.immediate16();
	if (!ram.IsValidRange(addr, 4)) {
		return ExecutionStatus::MemoryOutOfBounds;
	}
	ram.WriteWord(addr, state.GetRegister(instruction.a), tick);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteSxt(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	uint32_t val = state.GetRegister(instruction.b);
	uint32_t imm = instruction.c;
	uint32_t res = val;
	if (imm == 0) {
		uint32_t signBit = (val >> 7) & 1;
		uint32_t mask = (0 - signBit) << 8; // 0xFFFFFF00 если знаковый бит равен 1, иначе 0x00000000
		res = (val & 0xFF) | mask;
	} else if (imm == 1) {
		uint32_t signBit = (val >> 15) & 1;
		uint32_t mask = (0 - signBit) << 16;
		res = (val & 0xFFFF) | mask;
	}

	state.SetRegister(instruction.a, res);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteBswap(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	uint32_t val = state.GetRegister(instruction.b);
	uint32_t imm = instruction.c;
	uint32_t res = val;
	if (imm == 1) {
		uint32_t low = val & 0xFF;
		uint32_t high = (val >> 8) & 0xFF;
		res = (val & 0xFFFF0000) | (low << 8) | high;
	} else if (imm == 2) {
		uint32_t b0 = val & 0xFF;
		uint32_t b1 = (val >> 8) & 0xFF;
		uint32_t b2 = (val >> 16) & 0xFF;
		uint32_t b3 = (val >> 24) & 0xFF;
		res = (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;
	}
	state.SetRegister(instruction.a, res);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteScrw(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	state.SetScr(instruction.a, state.GetRegister(instruction.b));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteScrr(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	state.SetRegister(instruction.a, state.GetScr(instruction.b));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteAspi(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	int16_t sI = static_cast<int16_t>(instruction.immediate16());
	uint32_t spSize = state.GetScr(2);
	if (sI < 0 && spSize < static_cast<uint32_t>(-sI)) {
		return ExecutionStatus::StackOverflow;
	}
	uint32_t spTop = state.GetScr(1);
	if (sI > 0) {
		ram.FreeMemoryRegion(spTop, static_cast<uint32_t>(sI));
	}
	state.SetScr(1, spTop + sI);
	state.SetScr(2, spSize + sI);
	state.SetRegister(instruction.a, spTop + sI);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteAspr(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	int32_t sI = static_cast<int32_t>(state.GetRegister(instruction.b));
	uint32_t spSize = state.GetScr(2);
	if (sI < 0 && spSize < static_cast<uint32_t>(-sI)) {
		return ExecutionStatus::StackOverflow;
	}
	uint32_t spTop = state.GetScr(1);
	if (sI > 0) {
		ram.FreeMemoryRegion(spTop, static_cast<uint32_t>(sI));
	}
	state.SetScr(1, spTop + sI);
	state.SetScr(2, spSize + sI);
	state.SetRegister(instruction.a, spTop + sI);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteRjmp(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	int16_t offset = static_cast<int16_t>(instruction.immediate16());
	// Поскольку Execute() увеличивает IP на 4 после успешного выполнения,
	// вычитаем 4 из целевого IP, чтобы компенсировать этот шаг.
	state.SetInstructionPointer(state.InstructionPointer() + offset * 4 - 4);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteBrz(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	if (state.GetRegister(instruction.a) == 0) {
		int16_t offset = static_cast<int16_t>(instruction.immediate16());
		state.SetInstructionPointer(state.InstructionPointer() + offset * 4 - 4);
	}
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteBrnz(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	if (state.GetRegister(instruction.a) != 0) {
		int16_t offset = static_cast<int16_t>(instruction.immediate16());
		state.SetInstructionPointer(state.InstructionPointer() + offset * 4 - 4);
	}
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteAjmp(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	state.SetInstructionPointer(state.GetRegister(instruction.a) - 4);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteCall(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	// Сохраняем адрес следующей инструкции в R1
	state.SetRegister(1, state.InstructionPointer() + 4);
	state.SetInstructionPointer(state.GetRegister(instruction.a) - 4);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteEq(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) == state.GetRegister(instruction.c) ? 1 : 0);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteNe(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) != state.GetRegister(instruction.c) ? 1 : 0);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteLt(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) < state.GetRegister(instruction.c) ? 1 : 0);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteGe(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	state.SetRegister(instruction.a, state.GetRegister(instruction.b) >= state.GetRegister(instruction.c) ? 1 : 0);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteSlt(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	int32_t valB = static_cast<int32_t>(state.GetRegister(instruction.b));
	int32_t valC = static_cast<int32_t>(state.GetRegister(instruction.c));
	state.SetRegister(instruction.a, valB < valC ? 1 : 0);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteSge(CpuState& state, Ram& ram, const Cpu::Isa::Instruction& instruction, uint32_t tick)
{
	int32_t valB = static_cast<int32_t>(state.GetRegister(instruction.b));
	int32_t valC = static_cast<int32_t>(state.GetRegister(instruction.c));
	state.SetRegister(instruction.a, valB >= valC ? 1 : 0);
	return ExecutionStatus::Success;
}
}