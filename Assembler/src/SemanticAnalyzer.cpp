#include "SemanticAnalyzer.h"

#include "Cpu/Isa/InstructionSet.h"
#include "Cpu/Isa/Register.h"

#include <charconv>
#include <cstdint>

namespace Assembler {
namespace {

std::string operandKindName(Cpu::Isa::OperandKind kind)
{
    switch (kind) {
    case Cpu::Isa::OperandKind::Register:
        return "register";
    case Cpu::Isa::OperandKind::Immediate16:
        return "16-bit immediate";
    }

    return "operand";
}

}

std::vector<Cpu::Isa::Instruction> SemanticAnalyzer::Analyze(
    const std::vector<ParsedInstruction>& parsedInstructions,
    DiagnosticBag& diagnostics) const
{
    std::vector<Cpu::Isa::Instruction> instructions;

    for (const auto& parsed : parsedInstructions) {
        const auto format = Cpu::Isa::findInstructionByMnemonic(parsed.Mnemonic);
        if (!format.has_value()) {
            diagnostics.Add(Diagnostic{
                DiagnosticSeverity::Error,
                DiagnosticCode::UnknownInstruction,
                parsed.Range,
                "unknown instruction \"" + parsed.Mnemonic + "\"",
            });
            continue;
        }

        if (parsed.Operands.size() < format->operands.size()) {
            diagnostics.Add(Diagnostic{
                DiagnosticSeverity::Error,
                DiagnosticCode::MissingOperand,
                parsed.Range,
                "instruction \"" + parsed.Mnemonic + "\" expects " + std::to_string(format->operands.size())
                    + " operands",
            });
            continue;
        }

        if (parsed.Operands.size() > format->operands.size()) {
            diagnostics.Add(Diagnostic{
                DiagnosticSeverity::Error,
                DiagnosticCode::TooManyOperands,
                parsed.Operands[format->operands.size()].Range,
                "too many operands for instruction \"" + parsed.Mnemonic + "\"",
            });
            continue;
        }

        Cpu::Isa::Instruction instruction;
        instruction.opcode = format->opcode;
        bool ok = true;

        for (std::size_t index = 0; index < format->operands.size(); ++index) {
            const auto expected = format->operands[index];
            const auto& operand = parsed.Operands[index];

            if (expected == Cpu::Isa::OperandKind::Register) {
                if (operand.Kind != ParsedOperandKind::Identifier) {
                    diagnostics.Add(Diagnostic{
                        DiagnosticSeverity::Error,
                        DiagnosticCode::WrongOperandType,
                        operand.Range,
                        "expected " + operandKindName(expected),
                    });
                    ok = false;
                    continue;
                }

                const auto reg = Cpu::Isa::parseRegister(operand.Text);
                if (!reg.has_value()) {
                    diagnostics.Add(Diagnostic{
                        DiagnosticSeverity::Error,
                        DiagnosticCode::InvalidRegister,
                        operand.Range,
                        "invalid register \"" + operand.Text + "\"",
                    });
                    ok = false;
                    continue;
                }

                if (index == 0) {
                    instruction.a = *reg;
                } else if (index == 1) {
                    instruction.b = *reg;
                } else if (index == 2) {
                    instruction.c = *reg;
                }
            } else if (expected == Cpu::Isa::OperandKind::Immediate16) {
                if (operand.Kind != ParsedOperandKind::Number) {
                    diagnostics.Add(Diagnostic{
                        DiagnosticSeverity::Error,
                        DiagnosticCode::WrongOperandType,
                        operand.Range,
                        "expected " + operandKindName(expected),
                    });
                    ok = false;
                    continue;
                }

                std::uint16_t immediate = 0;
                if (!parseImmediate16(operand, immediate, diagnostics)) {
                    ok = false;
                    continue;
                }

                instruction.b = static_cast<std::uint8_t>(immediate & 0x00FF);
                instruction.c = static_cast<std::uint8_t>((immediate >> 8) & 0x00FF);
            }
        }

        if (ok) {
            instructions.push_back(instruction);
        }
    }

    return instructions;
}

bool SemanticAnalyzer::parseImmediate16(const ParsedOperand& operand, std::uint16_t& value, DiagnosticBag& diagnostics)
{
    unsigned long parsed = 0;
    int base = 10;
    std::string_view text = operand.Text;
    if (text.size() > 2 && text[0] == '0' && (text[1] == 'x' || text[1] == 'X')) {
        text.remove_prefix(2);
        base = 16;
    }

    const auto* begin = text.data();
    const auto* end = begin + text.size();
    const auto result = std::from_chars(begin, end, parsed, base);

    if (result.ec != std::errc{} || result.ptr != end) {
        diagnostics.Add(Diagnostic{
            DiagnosticSeverity::Error,
            DiagnosticCode::InvalidImmediate,
            operand.Range,
            "invalid immediate value \"" + operand.Text + "\"",
        });
        return false;
    }

    if (parsed > 0xFFFFUL) {
        diagnostics.Add(Diagnostic{
            DiagnosticSeverity::Error,
            DiagnosticCode::ImmediateOutOfRange,
            operand.Range,
            "immediate value " + operand.Text + " is too large for 16-bit value",
        });
        return false;
    }

    value = static_cast<std::uint16_t>(parsed);
    return true;
}

}
