#include "Emulator.h"

#include "Executor.h"
#include "Cpu/Isa/Codec.h"

namespace Emulator
{
EmulatorResult Emulator::Run(const Cpu::Bytecode &bytecode) const
{
	const auto instructions = Cpu::Isa::decodeProgram(bytecode);
	if (!instructions.has_value())
	{
		return EmulatorResult{CpuState{}, false, "invalid bytecode"};
	}

	CpuState state;
	Executor executor;
	for (const auto &instruction : *instructions)
	{
		executor.Execute(state, instruction);
	}

	return EmulatorResult{state, true, ""};
}

}
