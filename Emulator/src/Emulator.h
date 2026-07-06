#pragma once

#include "CpuState.h"
#include "Tracer.h"
#include "Cpu/Common/Types.h"

#include <string>

namespace Emulator
{
struct EmulatorResult
{
	CpuState State;
	bool Success = true;
	std::string Error;
};

class Emulator
{
public:
	EmulatorResult Run(const Cpu::Bytecode &bytecode, bool traceEnabled = false, std::vector<TickInterval> traceIntervals = {}) const;
};

}
