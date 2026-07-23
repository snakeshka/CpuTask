#pragma once

#include "Cpu/Common/Types.h"

#include <cstdint>
#include <vector>

namespace Cpu {
using Byte = uint8_t;
using Word = uint16_t;
using DWord = uint32_t;

using Bytecode = std::vector<Byte>;
}