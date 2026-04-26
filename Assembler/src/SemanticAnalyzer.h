#pragma once

#include "DiagnosticBag.h"
#include "ParsedInstruction.h"
#include "Cpu/Isa/Instruction.h"

#include <vector>

namespace Assembler {

class SemanticAnalyzer {
public:
    std::vector<Cpu::Isa::Instruction> Analyze(
        const std::vector<ParsedInstruction>& parsedInstructions,
        DiagnosticBag& diagnostics) const;

private:
    static bool parseImmediate16(const ParsedOperand& operand, std::uint16_t& value, DiagnosticBag& diagnostics);
};

}
