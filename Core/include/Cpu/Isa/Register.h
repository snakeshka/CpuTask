#pragma once

#include "Cpu/Common/Types.h"

#include <cstdint>
#include <optional>
#include <string_view>

namespace Cpu::Isa
{
using RegisterIndex = uint8_t;

constexpr RegisterIndex RegisterCount = 16;

bool isValidRegister(RegisterIndex index);

std::optional<RegisterIndex> parseRegister(std::string_view text);

}