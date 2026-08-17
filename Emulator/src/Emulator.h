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

struct RamConfig
{
	uint32_t Size = 65536;
	bool WarnUninit = false;
	bool ClearStack = false;
	bool TraceEnabled = false;
	std::vector<AddrInterval> TraceIntervals;
};

struct TraceConfig
{
	bool Disasm = false;
	std::vector<TickInterval> Intervals;
	bool Interrupts = false;
	bool Pic = false;
};

struct EmulatorConfig
{
	RamConfig Ram;
	TraceConfig Trace;
};

class Emulator
{
public:
	EmulatorResult Run(
		const Cpu::Bytecode& bytecode,
		const EmulatorConfig& config = {}) const;
};
}