#pragma once

#include <array>
#include <cstdint>
#include <cstddef>

namespace Emulator
{
class CpuState
{
public:
	uint32_t GetRegister(size_t index) const;
	void SetRegister(size_t index, uint32_t value);

	uint32_t ProgramCounter() const;
	void SetProgramCounter(uint32_t pc);
	void AdvanceProgramCounter();

private:
	std::array<uint32_t, 16> m_Registers{};
	uint32_t m_Pc = 0;
};

}
