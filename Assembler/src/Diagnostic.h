#pragma once

#include <cstddef>
#include <string>
#include <cstdint>

namespace Assembler {
enum class DiagnosticSeverity : uint8_t
{
	Error,
	Warning,
	Note
};

enum class DiagnosticCode : uint8_t
{
	UnknownCharacter,
	InvalidNumberLiteral,
	UnknownInstruction,
	InvalidRegister,
	InvalidImmediate,
	ImmediateOutOfRange,
	UnexpectedToken,
	MissingOperand,
	TooManyOperands,
	WrongOperandType,
	CannotOpenInputFile,
	CannotWriteOutputFile,
	InternalAssemblerError,
	DuplicateLabel,
	UndefinedLabel,
	InvalidLabelUsage
};

struct SourceLocation
{
	size_t Line = 1;
	size_t Column = 1;
};

struct SourceRange
{
	SourceLocation Begin;
	SourceLocation End;
};

struct Diagnostic
{
	DiagnosticSeverity Severity{};
	DiagnosticCode Code{};
	SourceRange Range{};
	std::string Message;
};

std::string_view ToString(DiagnosticSeverity severity);
}