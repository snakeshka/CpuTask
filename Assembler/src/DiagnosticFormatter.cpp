#include "DiagnosticFormatter.h"

#include <algorithm>
#include <sstream>

namespace Assembler {

std::string DiagnosticFormatter::format(const SourceFile& sourceFile, const Diagnostic& diagnostic) const
{
    std::ostringstream output;
    output << sourceFile.Path() << ':' << diagnostic.Range.Begin.Line << ':' << diagnostic.Range.Begin.Column
           << ": " << ToString(diagnostic.Severity) << ": " << diagnostic.Message << '\n';

    const auto line = sourceFile.GetLine(diagnostic.Range.Begin.Line);
    if (!line.empty()) {
        output << "    " << line << '\n';
        output << "    ";

        const auto startColumn = diagnostic.Range.Begin.Column;
        const auto endColumn = std::max(diagnostic.Range.End.Column, startColumn + 1);
        for (std::size_t column = 1; column < startColumn; ++column) {
            output << ' ';
        }
        for (std::size_t column = startColumn; column < endColumn; ++column) {
            output << '^';
        }
        output << '\n';
    }

    return output.str();
}

}
