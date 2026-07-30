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
			case Cpu::Isa::OperandKind::SystemRegister:
				return "system register";
			case Cpu::Isa::OperandKind::Immediate16:
				return "16-bit immediate";
			case Cpu::Isa::OperandKind::UpperImmediate16:
				return "16-bit upper immediate";
		}

		return "operand";
	}
}

std::vector<Cpu::Isa::Instruction> SemanticAnalyzer::Analyze(
	const std::vector<Statement>& statements,
	DiagnosticBag& diagnostics) const
{
	// ==========================================
	// ПРОХОД 1: Сбор меток и расчет их адресов
	// ==========================================
	std::unordered_map<std::string, uint32_t> symbolTable;
	uint32_t currentAddress = 0;
	for (const auto& stmt : statements) {
		if (stmt.Kind == StatementKind::Label) {
			if (symbolTable.contains(stmt.LabelName)) {
				diagnostics.Add(Diagnostic{
					DiagnosticSeverity::Error,
					DiagnosticCode::DuplicateLabel,
					stmt.Range,
					"duplicate label definition \"" + stmt.LabelName + "\"",
				});
			} else {
				symbolTable[stmt.LabelName] = currentAddress;
			}
		} else if (stmt.Kind == StatementKind::Instruction) {
			currentAddress += 4;
		}
	}
	if (diagnostics.HasErrors()) {
		return {};
	}
	// ==========================================
	// ПРОХОД 2: Разбор инструкций и подстановка меток
	// ==========================================
	std::vector<Cpu::Isa::Instruction> instructions;
	uint32_t instructionAddress = 0;
	for (const auto& stmt : statements) {
		if (stmt.Kind == StatementKind::Label) {
			continue;
		}
		const auto& parsed = stmt.Instruction;
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
				"instruction \"" + parsed.Mnemonic + "\" expects " + std::to_string(format->operands.size()) + " operands",
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
		for (size_t index = 0; index < format->operands.size(); ++index) {
			const auto expected = format->operands[index];
			const auto& operand = parsed.Operands[index];
			if (expected == Cpu::Isa::OperandKind::Register) {
				if (operand.Kind != ParsedOperandKind::Identifier) {
					diagnostics.Add(Diagnostic{DiagnosticSeverity::Error, DiagnosticCode::WrongOperandType, operand.Range, "expected register"});
					ok = false;
					continue;
				}
				const auto reg = Cpu::Isa::parseRegister(operand.Text);
				if (!reg.has_value()) {
					diagnostics.Add(Diagnostic{DiagnosticSeverity::Error, DiagnosticCode::InvalidRegister, operand.Range, "invalid register \"" + operand.Text + "\""});
					ok = false;
					continue;
				}
				if (index == 0)
					instruction.a = *reg;
				else if (index == 1)
					instruction.b = *reg;
				else if (index == 2)
					instruction.c = *reg;
			} else if (expected == Cpu::Isa::OperandKind::SystemRegister) {
				if (operand.Kind != ParsedOperandKind::Identifier) {
					diagnostics.Add(Diagnostic{DiagnosticSeverity::Error, DiagnosticCode::WrongOperandType, operand.Range, "expected system register"});
					ok = false;
					continue;
				}
				uint8_t scrIndex = 0;
				if (operand.Text == "IP")
					scrIndex = 0;
				else if (operand.Text == "SP_TOP")
					scrIndex = 1;
				else if (operand.Text == "SP_SIZE")
					scrIndex = 2;
				else if (operand.Text == "IIP")
					scrIndex = 3;
				else if (operand.Text == "ICR")
					scrIndex = 4;
				else if (operand.Text == "IHR")
					scrIndex = 5;
				else {
					diagnostics.Add(Diagnostic{DiagnosticSeverity::Error, DiagnosticCode::InvalidRegister, operand.Range, "invalid system register \"" + operand.Text + "\""});
					ok = false;
					continue;
				}
				if (index == 0)
					instruction.a = scrIndex;
				else if (index == 1)
					instruction.b = scrIndex;
				else if (index == 2)
					instruction.c = scrIndex;
			} else if (expected == Cpu::Isa::OperandKind::Immediate16 || expected == Cpu::Isa::OperandKind::UpperImmediate16) {
				uint16_t immediate = 0;
				if (operand.Kind == ParsedOperandKind::Number) {
					// Разбор числа
					if (!parseImmediate16(operand, immediate, diagnostics)) {
						ok = false;
						continue;
					}
				} else if (operand.Kind == ParsedOperandKind::Identifier) {
					// Если передано имя регистра (R0..R15) на место числа, это ошибка типа операнда
					if (Cpu::Isa::parseRegister(operand.Text).has_value()) {
						diagnostics.Add(Diagnostic{
							DiagnosticSeverity::Error,
							DiagnosticCode::WrongOperandType,
							operand.Range,
							"expected " + operandKindName(expected),
						});
						ok = false;
						continue;
					}
					// Иначе это имя метки (например, MY_LABEL или abc)
					if (parsed.Mnemonic != "LI" && parsed.Mnemonic != "LUI" &&
						parsed.Mnemonic != "RJMP" && parsed.Mnemonic != "BRZ" && parsed.Mnemonic != "BRNZ") {
						diagnostics.Add(Diagnostic{
							DiagnosticSeverity::Error,
							DiagnosticCode::InvalidLabelUsage,
							operand.Range,
							"label \"" + operand.Text + "\" cannot be used in instruction \"" + parsed.Mnemonic + "\"",
						});
						ok = false;
						continue;
					}
					auto it = symbolTable.find(operand.Text);
					if (it == symbolTable.end()) {
						diagnostics.Add(Diagnostic{
							DiagnosticSeverity::Error,
							DiagnosticCode::UndefinedLabel,
							operand.Range,
							"undefined label \"" + operand.Text + "\"",
						});
						ok = false;
						continue;
					}
					uint32_t labelAddr = it->second;
					if (parsed.Mnemonic == "LI") {
						immediate = static_cast<uint16_t>(labelAddr & 0xFFFF);
					} else if (parsed.Mnemonic == "LUI") {
						immediate = static_cast<uint16_t>((labelAddr >> 16) & 0xFFFF);
					} else {
						// Относительный переход для RJMP, BRZ, BRNZ
						int32_t offset = (static_cast<int32_t>(labelAddr) - static_cast<int32_t>(instructionAddress)) / 4;
						if (offset < -32768 || offset > 32767) {
							diagnostics.Add(Diagnostic{
								DiagnosticSeverity::Error,
								DiagnosticCode::ImmediateOutOfRange,
								operand.Range,
								"relative jump offset to label \"" + operand.Text + "\" is out of 16-bit range",
							});
							ok = false;
							continue;
						}
						immediate = static_cast<uint16_t>(static_cast<int16_t>(offset));
					}
				}
				instruction.b = static_cast<uint8_t>(immediate & 0x00FF);
				instruction.c = static_cast<uint8_t>((immediate >> 8) & 0x00FF);
			}
		}
		if (ok) {
			instructions.push_back(instruction);
			instructionAddress += 4;
		}
	}
	return instructions;
}

bool SemanticAnalyzer::parseImmediate16(const ParsedOperand& operand, uint16_t& value, DiagnosticBag& diagnostics)
{
	long long parsed = 0;
	int base = 10;
	std::string_view text = operand.Text;
	bool negative = false;
	if (text.starts_with('-')) {
		negative = true;
		text.remove_prefix(1);
	}
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

	if (negative) {
		parsed = -parsed;
	}

	if (parsed < -0x8000LL || parsed > 0xFFFFLL) {
		diagnostics.Add(Diagnostic{
			DiagnosticSeverity::Error,
			DiagnosticCode::ImmediateOutOfRange,
			operand.Range,
			"immediate value " + operand.Text + " is too large for 16-bit value",
		});
		return false;
	}

	value = static_cast<uint16_t>(parsed);
	return true;
}
}