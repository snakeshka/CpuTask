#include "Emulator.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <optional>

namespace
{
std::optional<Cpu::Bytecode> readBytecode(const std::string &path)
{
	std::ifstream input(path, std::ios::binary);
	if (!input)
	{
		return std::nullopt;
	}

	return Cpu::Bytecode(
		std::istreambuf_iterator<char>(input),
		std::istreambuf_iterator<char>());
}

}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		std::cerr << "usage: Emulator program.o\n";
		return 1;
	}

	const auto bytecode = readBytecode(argv[1]);
	if (!bytecode.has_value())
	{
		std::cerr << argv[1] << ": error: cannot open input file\n";
		return 1;
	}

	Emulator::Emulator emulator;
	const auto result = emulator.Run(*bytecode);
	if (!result.Success)
	{
		std::cerr << "error: " << result.Error << '\n';
		return 1;
	}

	for (std::size_t index = 0; index < 16; ++index)
	{
		std::cout << 'R' << index << "=0x"
				  << std::hex << std::uppercase << std::setw(8) << std::setfill('0')
				  << result.State.GetRegister(index) << std::dec << '\n';
	}

	return 0;
}
