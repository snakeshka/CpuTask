#pragma once

#include <cstdint>

namespace Cpu::Isa {

enum class Opcode : std::uint8_t {
    LI = 0x00,
    ADD = 0x02
};

}
