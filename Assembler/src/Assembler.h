#pragma once

#include "Diagnostic.h"
#include "Cpu/Common/Types.h"

#include <string_view>
#include <vector>

namespace Assembler {
struct AssembleResult
{
	Cpu::Bytecode Bytecode;
	std::vector<Diagnostic> Diagnostics;

	bool Success() const;
};

class Assembler
{
public:
	AssembleResult Assemble(std::string_view source) const;
};
}