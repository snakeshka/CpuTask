#include "Assembler.h"
#include "CliOptions.h"
#include "DiagnosticFormatter.h"
#include "SourceFile.h"

#include <fstream>
#include <iostream>

int main(int argc, char* argv[])
{
    const auto options = Assembler::ParseCliOptions(argc, argv);
    if (!options.has_value()) {
        std::cerr << "usage: Assembler input.s output.o\n";
        return 1;
    }

    auto sourceFile = Assembler::SourceFile::readFromDisk(options->InputPath);
    if (!sourceFile.has_value()) {
        std::cerr << options->InputPath << ": error: cannot open input file\n";
        return 1;
    }

    Assembler::Assembler assembler;
    const auto result = assembler.Assemble(sourceFile->Text());
    if (!result.Success()) {
        Assembler::DiagnosticFormatter formatter;
        for (const auto& diagnostic : result.Diagnostics) {
            std::cerr << formatter.format(*sourceFile, diagnostic);
        }
        return 1;
    }

    std::ofstream output(options->OutputPath, std::ios::binary);
    if (!output) {
        std::cerr << options->OutputPath << ": error: cannot write output file\n";
        return 1;
    }

    output.write(reinterpret_cast<const char*>(result.Bytecode.data()), static_cast<std::streamsize>(result.Bytecode.size()));
    if (!output) {
        std::cerr << options->OutputPath << ": error: cannot write output file\n";
        return 1;
    }

    return 0;
}
