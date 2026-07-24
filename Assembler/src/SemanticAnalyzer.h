#pragma once

#include "DiagnosticBag.h"
#include "ParsedInstruction.h"
#include "Cpu/Isa/Instruction.h"

#include <vector>
#include <unordered_map>

namespace Assembler {
class SemanticAnalyzer
{
public:
	std::vector<Cpu::Isa::Instruction> Analyze(
		const std::vector<Statement>& statements,
		DiagnosticBag& diagnostics) const;

private:
	static bool parseImmediate16(const ParsedOperand& operand, uint16_t& value, DiagnosticBag& diagnostics);
};
}