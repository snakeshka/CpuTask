#include "CpuState.h"

namespace Emulator
{
uint32_t CpuState::GetRegister(size_t index) const
{
	return m_Registers[index];
}
void CpuState::SetRegister(size_t index, uint32_t value)
{
	m_Registers[index] = value;
}
uint32_t CpuState::InstructionPointer() const
{
	return m_Ip;
}
void CpuState::SetInstructionPointer(uint32_t ip)
{
	m_Ip = ip;
}
void CpuState::AdvanceInstructionPointer()
{
	m_Ip += 4;
}

}
