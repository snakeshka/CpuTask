#include "Emulator.h"

#include <iostream>
#include <sstream>

#include "Executor.h"
#include "Tracer.h"
#include "BusRouter.h"
#include "EventQueue.h"

#include "Devices/OutputDevice.h"
#include "Devices/TimerDevice.h"
#include "Devices/PicDevice.h"

#include "Cpu/Isa/Codec.h"

namespace Emulator {
EmulatorResult Emulator::Run(const Cpu::Bytecode& bytecode,
							 const EmulatorConfig& config) const
{
	if (bytecode.size() % 4 != 0) [[unlikely]] {
		return EmulatorResult{CpuState{}, false, "invalid bytecode size"};
	}

	if (bytecode.size() > config.Ram.Size) [[unlikely]] {
		return EmulatorResult{CpuState{}, false, "program size exceeds RAM size"};
	}

	Ram ram(config.Ram.Size, config.Ram.WarnUninit, config.Ram.ClearStack);
	ram.SetTraceEnabled(config.Ram.TraceEnabled, config.Ram.TraceIntervals);

	EventQueue eventQueue;

	BusRouter bus(ram);
	PicDevice pic(config.Trace.Pic);
	TimerDevice timer(bus);
	OutputDevice outputDev(bus, eventQueue);

	bus.RegisterDevice(&outputDev);
	bus.RegisterDevice(&timer);
	bus.RegisterDevice(&pic);

	uint32_t tick = 1;
	for (size_t i = 0; i < bytecode.size(); i += 4) {
		const uint32_t word = static_cast<uint32_t>(bytecode[i]) |
		                     (static_cast<uint32_t>(bytecode[i + 1]) << 8) |
		                     (static_cast<uint32_t>(bytecode[i + 2]) << 16) |
		                     (static_cast<uint32_t>(bytecode[i + 3]) << 24);
		bus.WriteWord(static_cast<uint32_t>(i), word, 0);
	}

	CpuState state;
	state.SetInstructionPointer(0);
	Executor executor;
	Tracer tracer(config.Trace.Disasm, config.Trace.Intervals, config.Trace.Interrupts);

	bool isWaiting = false;

	while (state.InstructionPointer() < bytecode.size()) {
		bus.Tick(tick);
		eventQueue.ProcessEvents(tick);

		if (pic.HasPendingInterrupt() && state.GetScr(SystemRegister::IHR) != 0) {
			isWaiting = false;
			uint32_t currentIp = state.InstructionPointer();
			pic.AcknowledgeInterrupt();
			state.SetScr(SystemRegister::IIP, currentIp);
			state.SetScr(SystemRegister::ICR, static_cast<uint8_t>(InterruptCause::ExternalInterrupt));
			uint32_t ihr = state.GetScr(5);
		
			tracer.TraceInterrupt(tick, currentIp, 3, ihr);
		
			state.SetInstructionPointer(ihr);
			tick++;
			continue;
		}

		if (isWaiting) {
			tick++;
			continue;
		}

		uint32_t currentIp = state.InstructionPointer();

		const uint32_t rawWord = bus.ReadWord(currentIp, tick);
		const auto opcode = static_cast<Cpu::Isa::Opcode>(rawWord & 0xFF);

		// Ошибка декодирования (неизвестный опкод)
		if (!Cpu::Isa::OpcodeTable[static_cast<size_t>(opcode)].has_value()) [[unlikely]] {
			uint32_t cause = 0;             // Illegal Opcode
			state.SetScr(SystemRegister::IIP, currentIp);
			state.SetScr(SystemRegister::ICR, cause);
			uint32_t ihr = state.GetScr(SystemRegister::IHR);
			if (ihr == 0) {
				return EmulatorResult{state, false, "unhandled illegal opcode at address 0x" + std::string(1, currentIp)};
			}
			tracer.TraceInterrupt(tick, currentIp, cause, ihr);
			state.SetInstructionPointer(ihr);
			tick++;
			continue;
		}

		Cpu::Isa::Instruction instruction{
			.opcode = opcode,
			.a = static_cast<uint8_t>((rawWord >> 8) & 0xFF),
			.b = static_cast<uint8_t>((rawWord >> 16) & 0xFF),
			.c = static_cast<uint8_t>((rawWord >> 24) & 0xFF)};

		tracer.SaveStateBefore(state, instruction);
		auto status = executor.Execute(state, bus, tracer, instruction, tick);
		if (status == ExecutionStatus::HaltedPassed) {
			tracer.TraceStep(state, instruction);
			return EmulatorResult{state, true, ""};
		}
		if (status == ExecutionStatus::HaltedFailed) {
			tracer.TraceStep(state, instruction);
			return EmulatorResult{state, false, "program halted with failure status"};
		}
		if (status == ExecutionStatus::Wait) {
			tracer.TraceStep(state, instruction);
			isWaiting = true;
			tick++;
			continue;
		}
		// Ошибка выполнения инструкции
		if (status != ExecutionStatus::Success) [[unlikely]] {
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
			state.SetScr(SystemRegister::IIP, currentIp);
			state.SetScr(SystemRegister::ICR, cause);
			uint32_t ihr = state.GetScr(SystemRegister::IHR);
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