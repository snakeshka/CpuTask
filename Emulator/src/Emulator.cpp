#include "Emulator.h"

#include <iostream>
#include <sstream>

#include "Executor.h"
#include "Tracer.h"
#include "Cpu/Isa/Codec.h"

namespace Emulator
{
EmulatorResult Emulator::Run(const Cpu::Bytecode &bytecode, bool traceEnabled, std::vector<TickInterval> traceIntervals, uint32_t ramSize, bool warnUninit, std::vector<AddrInterval> ramIntervals, bool ramTraceEnabled, bool clearStack) const
{
	if (bytecode.size() % 4 != 0)
	{
		return EmulatorResult{CpuState{}, false, "invalid bytecode size"};
	}

	if (bytecode.size() > ramSize)
	{
		return EmulatorResult{CpuState{}, false, "program size exceeds RAM size"};
	}

	Ram ram(ramSize, warnUninit);
	ram.SetTraceEnabled(ramTraceEnabled, ramIntervals);

	uint32_t tick = 1;
	for (size_t i = 0; i < bytecode.size(); ++i)
	{
		ram.WriteByte(static_cast<uint32_t>(i), bytecode[i], 0);
	}
	CpuState state;
	state.SetInstructionPointer(0);
	Executor executor;
	Tracer tracer(traceEnabled, std::move(traceIntervals));

	while (state.InstructionPointer() < bytecode.size())
	{
		uint32_t currenIp = state.InstructionPointer();

		Cpu::Isa::InstructionBytes bytes;
		for (uint32_t offset = 0; offset < 4; ++offset)
		{
			bytes[offset] = ram.ReadByte(currenIp + offset, tick);
		}

		auto instrOpt = Cpu::Isa::decodeInstruction(bytes);
		if (!instrOpt.has_value())
		{
			return EmulatorResult{state, false, "invalid instruction encoding at IP 0x" + std::string(1, currenIp)};
		}
		const auto &instruction = *instrOpt;
		tracer.SaveStateBefore(state, instruction);

		auto status = executor.Execute(state, ram, instruction, tick);
		if (status != ExecutionStatus::Success)
		{
			auto formatOpt = Cpu::Isa::findInstructionByOpcode(instruction.opcode);
			std::string mnemonic = formatOpt.has_value() ? std::string(formatOpt->mnemonic) : "UNKNOWN";
			std::ostringstream errorMsg;
			errorMsg << ToString(status) << " in " << mnemonic << " at address 0x" << std::hex << currenIp;
			return EmulatorResult{state, false, errorMsg.str()};
		}
		tracer.TraceStep(state, instruction);
		tick++;
	}
	return EmulatorResult{state, true, ""};
}

}
