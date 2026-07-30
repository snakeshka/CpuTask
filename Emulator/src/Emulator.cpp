#include "Emulator.h"

#include <iostream>
#include <sstream>

#include "Executor.h"
#include "Tracer.h"
#include "Cpu/Isa/Codec.h"

namespace Emulator {
EmulatorResult Emulator::Run(const Cpu::Bytecode& bytecode,
                             bool traceEnabled,
                             std::vector<TickInterval> traceIntervals,
                             uint32_t ramSize,
                             bool warnUninit,
                             std::vector<AddrInterval> ramIntervals,
                             bool ramTraceEnabled,
                             bool clearStack,
                             bool traceIntEnabled) const
{
	if (bytecode.size() % 4 != 0) {
		return EmulatorResult{CpuState{}, false, "invalid bytecode size"};
	}

	if (bytecode.size() > ramSize) {
		return EmulatorResult{CpuState{}, false, "program size exceeds RAM size"};
	}

	Ram ram(ramSize, warnUninit, clearStack);
	ram.SetTraceEnabled(ramTraceEnabled, ramIntervals);

	uint32_t tick = 1;
	for (size_t i = 0; i < bytecode.size(); ++i) {
		ram.WriteByte(static_cast<uint32_t>(i), bytecode[i], 0);
	}
	CpuState state;
	state.SetInstructionPointer(0);
	Executor executor;
	Tracer tracer(traceEnabled, std::move(traceIntervals));

	while (state.InstructionPointer() < bytecode.size()) {
		uint32_t currentIp = state.InstructionPointer();

		Cpu::Isa::InstructionBytes bytes;
		for (uint32_t offset = 0; offset < 4; ++offset) {
			bytes[offset] = ram.ReadByte(currentIp + offset, tick);
		}

		auto instrOpt = Cpu::Isa::decodeInstruction(bytes);
		// Ошибка декодирования (неизвестный опкод)
		if (!instrOpt.has_value()) {
			uint32_t cause = 0;             // Illegal Opcode
			state.SetScr(3, currentIp);     // IIP = IP (сохраняем всегда)
			state.SetScr(4, cause);         // ICR = 0
			uint32_t ihr = state.GetScr(5); // IHR
			if (ihr == 0) {
				return EmulatorResult{state, false, "unhandled illegal opcode at address 0x" + std::string(1, currentIp)};
			}
			tracer.TraceInterrupt(tick, currentIp, cause, ihr);
			state.SetInstructionPointer(ihr);
			tick++;
			continue;
		}
		const auto& instruction = *instrOpt;
		tracer.SaveStateBefore(state, instruction);
		auto status = executor.Execute(state, ram, instruction, tick);
		// Ошибка выполнения инструкции
		if (status != ExecutionStatus::Success) {
			uint32_t cause = 0;
			if (status == ExecutionStatus::IllegalInstruction) {
				cause = 0; // Illegal Opcode
			} else if (status == ExecutionStatus::DivisionByZero || status == ExecutionStatus::InvalidScrIndex) {
				cause = 1; // Illegal Operand
			} else if (status == ExecutionStatus::StackOverflow) {
				cause = 2; // Stack Fault
			} else {
				auto formatOpt = Cpu::Isa::findInstructionByOpcode(instruction.opcode);
				std::string mnemonic = formatOpt.has_value() ? std::string(formatOpt->mnemonic) : "UNKNOWN";
				std::ostringstream errorMsg;
				errorMsg << ToString(status) << " in " << mnemonic << " at address 0x" << std::hex << currentIp;
				return EmulatorResult{state, false, errorMsg.str()};
			}
			// Сохраняем IIP и ICR ВСЕГДА
			state.SetScr(3, currentIp);     // IIP = IP
			state.SetScr(4, cause);         // ICR = cause
			uint32_t ihr = state.GetScr(5); // IHR
			if (ihr == 0) {
				auto formatOpt = Cpu::Isa::findInstructionByOpcode(instruction.opcode);
				std::string mnemonic = formatOpt.has_value() ? std::string(formatOpt->mnemonic) : "UNKNOWN";
				std::ostringstream errorMsg;
				errorMsg << ToString(status) << " in " << mnemonic << " at address 0x" << std::hex << currentIp;
				return EmulatorResult{state, false, errorMsg.str()};
			}
			tracer.PrintTraceLine(state, instruction, true);
			tracer.TraceInterrupt(tick, currentIp, cause, ihr);
			state.SetInstructionPointer(ihr);
			tick++;
			continue;
		}
		tracer.TraceStep(state, instruction);
		tick++;
	}
	return EmulatorResult{state, true, ""};
}
}