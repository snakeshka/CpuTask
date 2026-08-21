#pragma once

#include <stdexcept>
#include <string>

#if defined(_MSC_VER)
#define CPU_NOINLINE __declspec(noinline)
#elif defined(__GNUC__) || defined(__clang__)
#define CPU_NOINLINE __attribute__((noinline))
#else
#define CPU_NOINLINE
#endif

namespace Cpu::Common {

[[noreturn]] CPU_NOINLINE inline void ThrowRuntimeError(const char* message)
{
	throw std::runtime_error(message);
}

[[noreturn]] CPU_NOINLINE inline void ThrowOutOfRange(const char* message)
{
	throw std::out_of_range(message);
}

[[noreturn]] CPU_NOINLINE inline void ThrowOutOfRange(const std::string& message)
{
	throw std::out_of_range(message);
}

}