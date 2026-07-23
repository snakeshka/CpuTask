#pragma once

#include <array>
#include <cstdint>
#include <cstddef>

#include "Cpu/Common/Assert.h"

namespace Emulator {
class CpuState
{
public:
	uint32_t GetRegister(size_t index) const
	{
		CPU_ASSERT(index < 16, "Register index must be less than 16!");
		return m_Registers[index];
	}

	void SetRegister(size_t index, uint32_t value)
	{
		m_Registers[index] = value;
	}

	uint32_t InstructionPointer() const
	{
		return m_Ip;
	}

	void SetInstructionPointer(uint32_t ip)
	{
		m_Ip = ip;
	}

	void AdvanceInstructionPointer()
	{
		m_Ip += 4;
	}

	uint32_t GetScr(size_t index) const
	{
		if (index == 0)
			return m_Ip;
		if (index == 1)
			return m_SpTop;
		if (index == 2)
			return m_SpSize;
		CPU_ASSERT(false, "Invalid SCR index!");
		return 0;
	}

	void SetScr(size_t index, uint32_t value)
	{
		if (index == 0)
			m_Ip = value;
		else if (index == 1)
			m_SpTop = value;
		else if (index == 2)
			m_SpSize = value;
		else
			CPU_ASSERT(false, "Invalid SCR index!");
	}

private:
	std::array<uint32_t, 16> m_Registers{};
	uint32_t m_Ip = 0;
	uint32_t m_SpTop = 0;
	uint32_t m_SpSize = 0;
};
}