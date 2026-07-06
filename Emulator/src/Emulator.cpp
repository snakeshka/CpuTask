#include "Emulator.h"

#include "Executor.h"
#include "Tracer.h"
#include "Cpu/Isa/Codec.h"

namespace Emulator
{
EmulatorResult Emulator::Run(const Cpu::Bytecode &bytecode, bool traceEnabled) const
{
	const auto instructions = Cpu::Isa::decodeProgram(bytecode);
	if (!instructions.has_value())
	{
		return EmulatorResult{CpuState{}, false, "invalid bytecode"};
	}

	CpuState state;
	Executor executor;
	Tracer tracer(traceEnabled);
	try
	{
		for (const auto &instruction : *instructions)
		{
			tracer.SaveStateBefore(state, instruction);

			executor.Execute(state, instruction);

			tracer.TraceStep(state, instruction);
		}
	}
	catch (const std::exception &e)
	{
		return EmulatorResult{state, false, e.what()};
	}
	return EmulatorResult{state, true, ""};
}

}
