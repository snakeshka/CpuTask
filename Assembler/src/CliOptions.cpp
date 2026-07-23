#include "CliOptions.h"

namespace Assembler {
std::optional<CliOptions> ParseCliOptions(int argc, char* argv[])
{
	if (argc != 3) {
		return std::nullopt;
	}

	return CliOptions{argv[1], argv[2]};
}
}