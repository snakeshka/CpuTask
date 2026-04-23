#include "Cpu/Isa/Register.h"

#include <charconv>

namespace Cpu::Isa
{

bool isValidRegister(RegisterIndex index)
{
	return index < RegisterCount;
}

std::optional<RegisterIndex> parseRegister(std::string_view text)
{
	if (text.size() < 2 || text.front() != 'R')
	{
		return std::nullopt;
	}

    const auto numberText = text.substr(1);
	unsigned value = 0;
	const auto *begin = numberText.data();
	const auto *end = begin + numberText.size();
	const auto result = std::from_chars(begin, end, value, 10);

	if (result.ec != std::errc{} || result.ptr != end || value >= RegisterCount)
	{
		return std::nullopt;
	}

	return static_cast<RegisterIndex>(value);
}

}