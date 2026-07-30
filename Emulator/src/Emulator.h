#pragma once

#include "CpuState.h"
#include "Tracer.h"
#include "Ram.h"
#include "Cpu/Common/Types.h"

#include <string>

namespace Emulator {
struct EmulatorResult
{
	CpuState State;
	bool Success = true;
	std::string Error;
};

class Emulator
{
public:
	EmulatorResult Run(
		const Cpu::Bytecode& bytecode,
		bool traceEnabled = false,
		std::vector<TickInterval> traceIntervals = {},
		uint32_t ramSize = 65536,
		bool warnUninit = false,
		std::vector<AddrInterval> ramIntervals = {},
		bool ramTraceEnabled = false,
		bool clearStack = false,
		bool traceIntEnabled = false) const;
};
}