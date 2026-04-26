#pragma once

#include <cstddef>
#include <string>

namespace Assembler {

enum class DiagnosticSeverity {
    Error,
    Warning,
    Note
};

enum class DiagnosticCode {
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
    InternalAssemblerError
};

struct SourceLocation {
    std::size_t Line = 1;
    std::size_t Column = 1;
};

struct SourceRange {
    SourceLocation Begin;
    SourceLocation End;
};

struct Diagnostic {
    DiagnosticSeverity Severity;
    DiagnosticCode Code;
    SourceRange Range;
    std::string Message;
};

std::string_view ToString(DiagnosticSeverity severity);

}
