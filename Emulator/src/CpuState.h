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

enum class InterruptCause : uint8_t
{
	IllegalOpcode = 0,
	IllegalOperand = 1,
	StackFault = 2,
	ExternalInterrupt = 3
};

class CpuState
{
public:
	static constexpr size_t RegisterCount = 16;
	static constexpr size_t SystemRegisterCount = 6;
	static constexpr uint32_t InstructionSize = 4;

	uint32_t GetRegister(size_t index) const
	{
		CPU_ASSERT(index < RegisterCount, "Register index must be less than 16!");
		return m_Registers[index];
	}

	void SetRegister(size_t index, uint32_t value)
	{
		m_Registers[index] = value;
	}

	uint32_t InstructionPointer() const
	{
		return m_Scr[static_cast<size_t>(SystemRegister::IP)];
	}

	void SetInstructionPointer(uint32_t ip)
	{
		m_Scr[static_cast<size_t>(SystemRegister::IP)] = ip;
	}

	void AdvanceInstructionPointer()
	{
		m_Scr[static_cast<size_t>(SystemRegister::IP)] += InstructionSize;
	}

	bool IsValidScr(size_t index) const
	{
		return index < SystemRegisterCount;
	}

	uint32_t GetScr(SystemRegister reg) const
	{
		return m_Scr[static_cast<size_t>(reg)];
	}

	void SetScr(SystemRegister reg, uint32_t value)
	{
		m_Scr[static_cast<size_t>(reg)] = value;
	}

	uint32_t GetScr(size_t index) const
	{
		CPU_ASSERT(IsValidScr(index), "Invalid SCR index!");
		return m_Scr[index];
	}
	void SetScr(size_t index, uint32_t value)
	{
		CPU_ASSERT(IsValidScr(index), "Invalid SCR index!");
		m_Scr[index] = value;
	}

private:
	std::array<uint32_t, RegisterCount> m_Registers{};
	std::array<uint32_t, SystemRegisterCount> m_Scr{};
};
}