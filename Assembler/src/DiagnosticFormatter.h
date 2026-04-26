#pragma once

#include "Diagnostic.h"
#include "SourceFile.h"

#include <string>

namespace Assembler {

class DiagnosticFormatter {
public:
    std::string format(const SourceFile& sourceFile, const Diagnostic& diagnostic) const;
};

}
