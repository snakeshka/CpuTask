#include "Executor.h"

#include <stdexcept>

#include "Cpu/Isa/Opcode.h"

namespace Emulator {
namespace {
	constexpr uint8_t toIdx(Cpu::Isa::Opcode op)
	{
		return static_cast<uint8_t>(op);
	}
}

ExecutionStatus Executor::Execute(CpuState& state, BusRouter& bus, Tracer& tracer, const Cpu::Isa::Instruction& instruction, uint32_t tick) const
{
	ExecutionContext ctx{state, bus, tracer, instruction, tick};
	return Execute(ctx);
}

ExecutionStatus Executor::Execute(ExecutionContext& ctx) const
{
	const auto opcode = static_cast<uint8_t>(ctx.Instruction.opcode);
	CPU_ASSERT(opcode < s_handlers.size(), "Opcode value exceeds handlers array boundary!");
	auto status = s_handlers[opcode](ctx);
	if (status == ExecutionStatus::Success || status == ExecutionStatus::Wait) {
		ctx.State.AdvanceInstructionPointer();
	}
	return status;
}

consteval std::array<Executor::Handler, 256> Executor::CreateHandlersMap()
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
	handlers[toIdx(Cpu::Isa::Opcode::RIH)] = &Executor::ExecuteRih;
	handlers[toIdx(Cpu::Isa::Opcode::WAIT)] = &Executor::ExecuteWait;
	handlers[toIdx(Cpu::Isa::Opcode::EQ)] = &Executor::ExecuteEq;
	handlers[toIdx(Cpu::Isa::Opcode::NE)] = &Executor::ExecuteNe;
	handlers[toIdx(Cpu::Isa::Opcode::LT)] = &Executor::ExecuteLt;
	handlers[toIdx(Cpu::Isa::Opcode::GE)] = &Executor::ExecuteGe;
	handlers[toIdx(Cpu::Isa::Opcode::SLT)] = &Executor::ExecuteSlt;
	handlers[toIdx(Cpu::Isa::Opcode::SGE)] = &Executor::ExecuteSge;
	handlers[toIdx(Cpu::Isa::Opcode::SIM_CTRL)] = &Executor::ExecuteSimCtrl;

	return handlers;
}

const std::array<Executor::Handler, 256> Executor::s_handlers = Executor::CreateHandlersMap();

ExecutionStatus Executor::ExecuteIllegal(ExecutionContext& ctx)
{
	(void)ctx;
	return ExecutionStatus::IllegalInstruction;
}

ExecutionStatus Executor::ExecuteLi(ExecutionContext& ctx)
{
	ctx.State.SetRegister(ctx.Instruction.a, ctx.Instruction.immediate16());
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteLui(ExecutionContext& ctx)
{
	ctx.State.SetRegister(ctx.Instruction.a, (ctx.State.GetRegister(ctx.Instruction.a) & 0x0000FFFF) | (static_cast<uint32_t>(ctx.Instruction.immediate16()) << 16));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteAdd(ExecutionContext& ctx)
{
	ctx.State.SetRegister(ctx.Instruction.a, ctx.State.GetRegister(ctx.Instruction.b) + ctx.State.GetRegister(ctx.Instruction.c));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteSub(ExecutionContext& ctx)
{
	ctx.State.SetRegister(ctx.Instruction.a, ctx.State.GetRegister(ctx.Instruction.b) - ctx.State.GetRegister(ctx.Instruction.c));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteAnd(ExecutionContext& ctx)
{
	ctx.State.SetRegister(ctx.Instruction.a, ctx.State.GetRegister(ctx.Instruction.b) & ctx.State.GetRegister(ctx.Instruction.c));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteOr(ExecutionContext& ctx)
{
	ctx.State.SetRegister(ctx.Instruction.a, ctx.State.GetRegister(ctx.Instruction.b) | ctx.State.GetRegister(ctx.Instruction.c));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteXor(ExecutionContext& ctx)
{
	ctx.State.SetRegister(ctx.Instruction.a, ctx.State.GetRegister(ctx.Instruction.b) ^ ctx.State.GetRegister(ctx.Instruction.c));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteSll(ExecutionContext& ctx)
{
	ctx.State.SetRegister(ctx.Instruction.a, ctx.State.GetRegister(ctx.Instruction.b) << (ctx.State.GetRegister(ctx.Instruction.c) & 0x1F));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteSrl(ExecutionContext& ctx)
{
	ctx.State.SetRegister(ctx.Instruction.a, ctx.State.GetRegister(ctx.Instruction.b) >> (ctx.State.GetRegister(ctx.Instruction.c) & 0x1F));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteSra(ExecutionContext& ctx)
{
	//ctx.State.SetRegister(ctx.Instruction.a, static_cast<int32_t>(ctx.State.GetRegister(ctx.Instruction.b)) >> (ctx.State.GetRegister(ctx.Instruction.c) & 0x1F));

	uint32_t value = ctx.State.GetRegister(ctx.Instruction.b);
	uint32_t shift = ctx.State.GetRegister(ctx.Instruction.c) & 0x1F;

	if (shift == 0) [[unlikely]] {
		ctx.State.SetRegister(ctx.Instruction.a, value);
		return ExecutionStatus::Success;
	}

	uint32_t signBit = (value >> 31) & 1;
	uint32_t fillMask = (0 - signBit) << (32 - shift);
	uint32_t result = (value >> shift) | fillMask;

	ctx.State.SetRegister(ctx.Instruction.a, result);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteMul(ExecutionContext& ctx)
{
	ctx.State.SetRegister(ctx.Instruction.a, ctx.State.GetRegister(ctx.Instruction.b) * ctx.State.GetRegister(ctx.Instruction.c));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteUdiv(ExecutionContext& ctx)
{
	uint32_t divisor = ctx.State.GetRegister(ctx.Instruction.c);
	if (divisor == 0) [[unlikely]] {
		return ExecutionStatus::DivisionByZero;
	}
	ctx.State.SetRegister(ctx.Instruction.a, ctx.State.GetRegister(ctx.Instruction.b) / divisor);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteSdiv(ExecutionContext& ctx)
{
	int32_t valB = static_cast<int32_t>(ctx.State.GetRegister(ctx.Instruction.b));
	int32_t valC = static_cast<int32_t>(ctx.State.GetRegister(ctx.Instruction.c));
	if (valC == 0) [[unlikely]]	{
		return ExecutionStatus::DivisionByZero;
	}
	ctx.State.SetRegister(ctx.Instruction.a, static_cast<uint32_t>(valB / valC));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteLdb(ExecutionContext& ctx)
{
	uint32_t addr = ctx.State.GetRegister(ctx.Instruction.b) + ctx.Instruction.c;
	if (!ctx.Bus.IsValidRange(addr, 1)) [[unlikely]] {
		return ExecutionStatus::MemoryOutOfBounds;
	}
	ctx.State.SetRegister(ctx.Instruction.a, ctx.Bus.ReadByte(addr, ctx.Tick));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteLdh(ExecutionContext& ctx)
{
	uint32_t addr = ctx.State.GetRegister(ctx.Instruction.b) + ctx.Instruction.c;
	if (!ctx.Bus.IsValidRange(addr, 2)) [[unlikely]] {
		return ExecutionStatus::MemoryOutOfBounds;
	}
	ctx.State.SetRegister(ctx.Instruction.a, ctx.Bus.ReadHalfword(addr, ctx.Tick));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteLdw(ExecutionContext& ctx)
{
	uint32_t addr = ctx.State.GetRegister(ctx.Instruction.b) + ctx.Instruction.c;
	if (!ctx.Bus.IsValidRange(addr, 4)) [[unlikely]] {
		return ExecutionStatus::MemoryOutOfBounds;
	}
	ctx.State.SetRegister(ctx.Instruction.a, ctx.Bus.ReadWord(addr, ctx.Tick));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteStb(ExecutionContext& ctx)
{
	uint32_t addr = ctx.State.GetRegister(ctx.Instruction.b) + ctx.Instruction.c;
	if (!ctx.Bus.IsValidRange(addr, 1)) [[unlikely]] {
		return ExecutionStatus::MemoryOutOfBounds;
	}
	ctx.Bus.WriteByte(addr, static_cast<uint8_t>(ctx.State.GetRegister(ctx.Instruction.a) & 0xFF), ctx.Tick);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteSth(ExecutionContext& ctx)
{
	uint32_t addr = ctx.State.GetRegister(ctx.Instruction.b) + ctx.Instruction.c;
	if (!ctx.Bus.IsValidRange(addr, 2)) [[unlikely]] {
		return ExecutionStatus::MemoryOutOfBounds;
	}
	ctx.Bus.WriteHalfword(addr, static_cast<uint16_t>(ctx.State.GetRegister(ctx.Instruction.a) & 0xFFFF), ctx.Tick);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteStw(ExecutionContext& ctx)
{
	uint32_t addr = ctx.State.GetRegister(ctx.Instruction.b) + ctx.Instruction.c;
	if (!ctx.Bus.IsValidRange(addr, 4)) [[unlikely]] {
		return ExecutionStatus::MemoryOutOfBounds;
	}
	ctx.Bus.WriteWord(addr, ctx.State.GetRegister(ctx.Instruction.a), ctx.Tick);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteSxt(ExecutionContext& ctx)
{
	uint32_t val = ctx.State.GetRegister(ctx.Instruction.b);
	uint32_t imm = ctx.Instruction.c;
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

	ctx.State.SetRegister(ctx.Instruction.a, res);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteBswap(ExecutionContext& ctx)
{
	uint32_t val = ctx.State.GetRegister(ctx.Instruction.b);
	uint32_t imm = ctx.Instruction.c;
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
	ctx.State.SetRegister(ctx.Instruction.a, res);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteScrw(ExecutionContext& ctx)
{
	if (!ctx.State.IsValidScr(ctx.Instruction.a)) [[unlikely]] {
		return ExecutionStatus::InvalidScrIndex;
	}
	ctx.State.SetScr(ctx.Instruction.a, ctx.State.GetRegister(ctx.Instruction.b));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteScrr(ExecutionContext& ctx)
{
	if (!ctx.State.IsValidScr(ctx.Instruction.b)) [[unlikely]] {
		return ExecutionStatus::InvalidScrIndex;
	}
	ctx.State.SetRegister(ctx.Instruction.a, ctx.State.GetScr(ctx.Instruction.b));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteAspi(ExecutionContext& ctx)
{
	int16_t sI = static_cast<int16_t>(ctx.Instruction.immediate16());
	uint32_t spSize = ctx.State.GetScr(SystemRegister::SP_SIZE);
	if (sI < 0 && spSize < static_cast<uint32_t>(-sI)) [[unlikely]] {
		return ExecutionStatus::StackOverflow;
	}
	uint32_t spTop = ctx.State.GetScr(SystemRegister::SP_TOP);
	if (sI > 0) {
		ctx.Bus.FreeMemoryRegion(spTop, static_cast<uint32_t>(sI));
	}
	ctx.State.SetScr(SystemRegister::SP_TOP, spTop + sI);
	ctx.State.SetScr(SystemRegister::SP_SIZE, spSize + sI);
	ctx.State.SetRegister(ctx.Instruction.a, spTop + sI);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteAspr(ExecutionContext& ctx)
{
	int32_t sI = static_cast<int32_t>(ctx.State.GetRegister(ctx.Instruction.b));
	uint32_t spSize = ctx.State.GetScr(SystemRegister::SP_SIZE);
	if (sI < 0 && spSize < static_cast<uint32_t>(-sI)) [[unlikely]] {
		return ExecutionStatus::StackOverflow;
	}
	uint32_t spTop = ctx.State.GetScr(SystemRegister::SP_TOP);
	if (sI > 0) {
		ctx.Bus.FreeMemoryRegion(spTop, static_cast<uint32_t>(sI));
	}
	ctx.State.SetScr(SystemRegister::SP_TOP, spTop + sI);
	ctx.State.SetScr(SystemRegister::SP_SIZE, spSize + sI);
	ctx.State.SetRegister(ctx.Instruction.a, spTop + sI);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteRjmp(ExecutionContext& ctx)
{
	int16_t offset = static_cast<int16_t>(ctx.Instruction.immediate16());
	// Поскольку Execute() увеличивает IP на CpuState::InstructionSize после успешного выполнения,
	// вычитаем CpuState::InstructionSize из целевого IP, чтобы компенсировать этот шаг.
	ctx.State.SetInstructionPointer(ctx.State.InstructionPointer() + offset * CpuState::InstructionSize - CpuState::InstructionSize);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteBrz(ExecutionContext& ctx)
{
	if (ctx.State.GetRegister(ctx.Instruction.a) == 0) {
		int16_t offset = static_cast<int16_t>(ctx.Instruction.immediate16());
		ctx.State.SetInstructionPointer(ctx.State.InstructionPointer() + offset * CpuState::InstructionSize - CpuState::InstructionSize);
	}
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteBrnz(ExecutionContext& ctx)
{
	if (ctx.State.GetRegister(ctx.Instruction.a) != 0) {
		int16_t offset = static_cast<int16_t>(ctx.Instruction.immediate16());
		ctx.State.SetInstructionPointer(ctx.State.InstructionPointer() + offset * CpuState::InstructionSize - CpuState::InstructionSize);
	}
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteAjmp(ExecutionContext& ctx)
{
	ctx.State.SetInstructionPointer(ctx.State.GetRegister(ctx.Instruction.a) - CpuState::InstructionSize);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteCall(ExecutionContext& ctx)
{
	// Сохраняем адрес следующей инструкции в R1
	ctx.State.SetRegister(1, ctx.State.InstructionPointer() + 4);
	ctx.State.SetInstructionPointer(ctx.State.GetRegister(ctx.Instruction.a) - CpuState::InstructionSize);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteRih(ExecutionContext& ctx)
{
	// IP = IIP (с учетом последующего Advance (+4) в Executor::Execute)
	ctx.State.SetInstructionPointer(ctx.State.GetScr(SystemRegister::IIP) - CpuState::InstructionSize);
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteWait(ExecutionContext& ctx)
{
	(void)ctx;
	return ExecutionStatus::Wait;
}

ExecutionStatus Executor::ExecuteEq(ExecutionContext& ctx)
{
	ctx.State.SetRegister(ctx.Instruction.a, static_cast<uint32_t>(ctx.State.GetRegister(ctx.Instruction.b) == ctx.State.GetRegister(ctx.Instruction.c)));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteNe(ExecutionContext& ctx)
{
	ctx.State.SetRegister(ctx.Instruction.a, static_cast<uint32_t>(ctx.State.GetRegister(ctx.Instruction.b) != ctx.State.GetRegister(ctx.Instruction.c)));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteLt(ExecutionContext& ctx)
{
	ctx.State.SetRegister(ctx.Instruction.a, static_cast<uint32_t>(ctx.State.GetRegister(ctx.Instruction.b) < ctx.State.GetRegister(ctx.Instruction.c)));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteGe(ExecutionContext& ctx)
{
	ctx.State.SetRegister(ctx.Instruction.a, static_cast<uint32_t>(ctx.State.GetRegister(ctx.Instruction.b) >= ctx.State.GetRegister(ctx.Instruction.c)));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteSlt(ExecutionContext& ctx)
{
	ctx.State.SetRegister(ctx.Instruction.a, static_cast<uint32_t>(ctx.State.GetRegister(ctx.Instruction.b) < ctx.State.GetRegister(ctx.Instruction.c)));
	return ExecutionStatus::Success;
}

ExecutionStatus Executor::ExecuteSge(ExecutionContext& ctx)
{
	ctx.State.SetRegister(ctx.Instruction.a, static_cast<uint32_t>(ctx.State.GetRegister(ctx.Instruction.b) >= ctx.State.GetRegister(ctx.Instruction.c)));
	return ExecutionStatus::Success;
}
ExecutionStatus Executor::ExecuteSimCtrl(ExecutionContext& ctx)
{
    // Команды вида 0xFF 0xFF <sub_opcode> <c>
    if (ctx.Instruction.a == 0xFF) {
        const uint8_t subOpcode = ctx.Instruction.b;
        const uint8_t regC = ctx.Instruction.c;
        if (subOpcode == 0x01) { 
            // _SIM_GET_TICK Rc
            ctx.State.SetRegister(regC, ctx.Tick);
            return ExecutionStatus::Success;
        } 
        
        if (subOpcode == 0x02) { 
            // _SIM_TRACE_DISASM Rc
            const uint32_t cond = ctx.State.GetRegister(regC);
            ctx.Tracer.SetTraceDisasmEnabled(cond != 0);
            return ExecutionStatus::Success;
        }
        if (subOpcode == 0x00) {
            // _SIM_HALTR Rc
            const uint32_t exitCode = ctx.State.GetRegister(regC);
            return (exitCode == 0) ? ExecutionStatus::HaltedPassed : ExecutionStatus::HaltedFailed;
        }
    } 
    else if (ctx.Instruction.a == 0x00) {
        // _SIM_HALTI I
        const uint16_t imm = ctx.Instruction.immediate16();
        return (imm == 0) ? ExecutionStatus::HaltedPassed : ExecutionStatus::HaltedFailed;
    }
    return ExecutionStatus::IllegalInstruction;
}
}