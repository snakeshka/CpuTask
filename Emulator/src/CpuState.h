#pragma once

#include <array>
#include <cstdint>
#include <cstddef>

#include "Cpu/Common/Assert.h"

namespace Emulator {
enum class SystemRegister : size_t
{
	IP = 0,      // Instruction Pointer
	SP_TOP = 1,  // Stack Pointer Top
	SP_SIZE = 2, // Stack Pointer Size
	IIP = 3,     // Interrupt Instruction Pointer
	ICR = 4,     // Interrupt Cause Register
	IHR = 5      // Interrupt Handler Register
};

class CpuState
{
public:
	static constexpr size_t RegisterCount = 16;
	static constexpr uint32_t InstructionSize = 4;

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
		m_Ip += InstructionSize;
	}

	bool IsValidScr(size_t index) const
	{
		return index < 6;
	}

	uint32_t GetScr(size_t index) const
	{
		switch (static_cast<SystemRegister>(index)) {
			case SystemRegister::IP:
				return m_Ip;
			case SystemRegister::SP_TOP:
				return m_SpTop;
			case SystemRegister::SP_SIZE:
				return m_SpSize;
			case SystemRegister::IIP:
				return m_Iip;
			case SystemRegister::ICR:
				return m_Icr;
			case SystemRegister::IHR:
				return m_Ihr;
			default:
				CPU_ASSERT(false, "Invalid SCR index!");
				return 0;
		}
	}

	void SetScr(size_t index, uint32_t value)
	{
		switch (static_cast<SystemRegister>(index)) {
			case SystemRegister::IP:
				m_Ip = value;
				break;
			case SystemRegister::SP_TOP:
				m_SpTop = value;
				break;
			case SystemRegister::SP_SIZE:
				m_SpSize = value;
				break;
			case SystemRegister::IIP:
				m_Iip = value;
				break;
			case SystemRegister::ICR:
				m_Icr = value;
				break;
			case SystemRegister::IHR:
				m_Ihr = value;
				break;
			default:
				CPU_ASSERT(false, "Invalid SCR index!");
				break;
		}
	}

private:
	std::array<uint32_t, RegisterCount> m_Registers{};
	uint32_t m_Ip = 0;     // SCR 0: IP
	uint32_t m_SpTop = 0;  // SCR 1: SP_TOP
	uint32_t m_SpSize = 0; // SCR 2: SP_SIZE
	uint32_t m_Iip = 0;    // SCR 3: IIP
	uint32_t m_Icr = 0;    // SCR 4: ICR
	uint32_t m_Ihr = 0;    // SCR 5: IHR
};
}