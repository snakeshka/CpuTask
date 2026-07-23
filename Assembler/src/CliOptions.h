#pragma once

#include <optional>
#include <string>

namespace Assembler {
struct CliOptions
{
	std::string InputPath;
	std::string OutputPath;
};

std::optional<CliOptions> ParseCliOptions(int argc, char* argv[]);
}