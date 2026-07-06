#include "Emulator.h"

#include <iostream>
#include <sstream>

#include "Executor.h"
#include "Tracer.h"
#include "Cpu/Isa/Codec.h"

namespace Emulator
{
EmulatorResult Emulator::Run(const Cpu::Bytecode &bytecode, bool traceEnabled, std::vector<TickInterval> traceIntervals) const
{
	const auto instructions = Cpu::Isa::decodeProgram(bytecode);
	if (!instructions.has_value())
	{
		return EmulatorResult{CpuState{}, false, "invalid bytecode"};
	}

	CpuState state;
	Executor executor;
	Tracer tracer(traceEnabled, std::move(traceIntervals));
	for (const auto &instruction : *instructions)
	{
		tracer.SaveStateBefore(state, instruction);

		auto status = executor.Execute(state, instruction);
		if (status != ExecutionStatus::Success)
		{
			auto formatOpt = Cpu::Isa::findInstructionByOpcode(instruction.opcode);
			std::string mnemonic = formatOpt.has_value() ? std::string(formatOpt->mnemonic) : "UNKNOWN";
			std::ostringstream errorMsg;
			errorMsg << ToString(status)
					 << " in " << mnemonic
					 << " at address 0x" << std::hex << state.ProgramCounter();
			return EmulatorResult{state, false, errorMsg.str()};
		}

		tracer.TraceStep(state, instruction);
	}
	return EmulatorResult{state, true, ""};
}

}
