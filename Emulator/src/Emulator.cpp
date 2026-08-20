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
	if (bytecode.size() % 4 != 0) {
		return EmulatorResult{CpuState{}, false, "invalid bytecode size"};
	}

	if (bytecode.size() > config.Ram.Size) {
		return EmulatorResult{CpuState{}, false, "program size exceeds RAM size"};
	}

	Ram ram(config.Ram.Size, config.Ram.WarnUninit, config.Ram.ClearStack);
	ram.SetTraceEnabled(config.Ram.TraceEnabled, config.Ram.TraceIntervals);

	BusRouter bus(ram);
	PicDevice pic(config.Trace.Pic);
	TimerDevice timer(bus);
	OutputDevice outputDev(bus);

	bus.RegisterDevice(&outputDev);
	bus.RegisterDevice(&timer);
	bus.RegisterDevice(&pic);

	uint32_t tick = 1;
	for (size_t i = 0; i < bytecode.size(); ++i) {
		bus.WriteByte(static_cast<uint32_t>(i), bytecode[i], 0);
	}
	CpuState state;
	state.SetInstructionPointer(0);
	Executor executor;
	Tracer tracer(config.Trace.Disasm, config.Trace.Intervals, config.Trace.Interrupts);

	EventQueue eventQueue;
	eventQueue.Schedule(outputDev.GetEventPeriod(), [&outputDev](uint32_t currentTick) -> uint32_t {
		return outputDev.OnEvent(currentTick);
	});

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
		const Cpu::Isa::InstructionBytes bytes = {
			static_cast<Cpu::Byte>(rawWord & 0xFF),
			static_cast<Cpu::Byte>((rawWord >> 8) & 0xFF),
			static_cast<Cpu::Byte>((rawWord >> 16) & 0xFF),
			static_cast<Cpu::Byte>((rawWord >> 24) & 0xFF),
		};

		auto instrOpt = Cpu::Isa::decodeInstruction(bytes);
		// Ошибка декодирования (неизвестный опкод)
		if (!instrOpt.has_value()) {
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
		const auto& instruction = *instrOpt;
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