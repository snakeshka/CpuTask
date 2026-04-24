#pragma once

#include <array>
#include <cstdint>
#include <cstddef>

namespace Emulator
{
class CpuState
{
public:
	std::uint32_t GetRegister(std::size_t index) const;
	void SetRegister(std::size_t index, std::uint32_t value);

	std::uint32_t ProgramCounter() const;
	void SetProgramCounter(std::uint32_t pc);
	void AdvanceProgramCounter();

private:
	std::array<std::uint32_t, 16> m_Registers{};
	std::uint32_t m_Pc = 0;
};

}
