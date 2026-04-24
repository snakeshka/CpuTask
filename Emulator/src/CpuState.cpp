#include "CpuState.h"

namespace Emulator
{
std::uint32_t CpuState::GetRegister(std::size_t index) const
{
	return m_Registers[index];
}

void CpuState::SetRegister(std::size_t index, std::uint32_t value)
{
	m_Registers[index] = value;
}

std::uint32_t CpuState::ProgramCounter() const
{
	return m_Pc;
}

void CpuState::SetProgramCounter(std::uint32_t pc)
{
	m_Pc = pc;
}

void CpuState::AdvanceProgramCounter()
{
	m_Pc += 4;
}

}
