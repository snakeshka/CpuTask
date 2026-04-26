#include "Assembler.h"

#include "DiagnosticBag.h"
#include "Lexer.h"
#include "Parser.h"
#include "SemanticAnalyzer.h"
#include "Cpu/Isa/Codec.h"

namespace Assembler {

bool AssembleResult::Success() const
{
    for (const auto& diagnostic : Diagnostics) {
        if (diagnostic.Severity == DiagnosticSeverity::Error) {
            return false;
        }
    }

    return true;
}

AssembleResult Assembler::Assemble(std::string_view source) const
{
    DiagnosticBag diagnostics;

    Lexer lexer(source);
    auto tokens = lexer.Lex(diagnostics);
    if (diagnostics.HasErrors()) {
        return AssembleResult{{}, diagnostics.diagnostics()};
    }

    Parser parser(tokens);
    auto parsedInstructions = parser.Parse(diagnostics);
    if (diagnostics.HasErrors()) {
        return AssembleResult{{}, diagnostics.diagnostics()};
    }

    SemanticAnalyzer analyzer;
    auto instructions = analyzer.Analyze(parsedInstructions, diagnostics);
    if (diagnostics.HasErrors()) {
        return AssembleResult{{}, diagnostics.diagnostics()};
    }

    return AssembleResult{Cpu::Isa::encodeProgram(instructions), diagnostics.diagnostics()};
}

}
