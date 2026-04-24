#pragma once

#include "CpuState.h"
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
	EmulatorResult Run(const Cpu::Bytecode &bytecode) const;
};

}
