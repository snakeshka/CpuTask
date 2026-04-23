#pragma once

#include "Cpu/Common/Types.h"

#include <cstdint>
#include <vector>

namespace Cpu
{

using Byte = std::uint8_t;
using Word = std::uint16_t;
using DWord = std::uint32_t;

using Bytecode = std::vector<Byte>;

}