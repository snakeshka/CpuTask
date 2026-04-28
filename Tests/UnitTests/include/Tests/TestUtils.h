#pragma once

#include "DiagnosticBag.h"

#include <algorithm>
#include <vector>

namespace Tests
{

inline bool HasDiagnosticCode(
    const std::vector<Assembler::Diagnostic> &diagnostics,
    Assembler::DiagnosticCode code)
{
    return std::ranges::any_of(diagnostics, [code](const Assembler::Diagnostic &diagnostic) {
        return diagnostic.Code == code;
    });
}

inline bool HasDiagnosticCode(
    const Assembler::DiagnosticBag &diagnostics,
    Assembler::DiagnosticCode code)
{
    return HasDiagnosticCode(diagnostics.diagnostics(), code);
}

}
