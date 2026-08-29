#include "OutputDevice.h"
#include <iostream>

namespace Emulator {
uint8_t OutputDevice::ReadByte(uint32_t offset)
{
	if (offset == 6) { // Регистр CTRL (0xE006)
		return m_Ctrl;
	}
	return 0;
}

void OutputDevice::WriteByte(uint32_t offset, uint8_t value) {
	if (offset == 6) { // CTRL register (0xE006)
		m_Ctrl = value;
	}
}

void OutputDevice::WriteHalfWord(uint32_t offset, uint16_t value) {
	if (offset == 4) { // SIZE register (0xE004)
		m_Size = value;
	}
}

void OutputDevice::WriteWord(uint32_t offset, uint32_t value)
{
	if (offset == 0) {	// DATA register (0xE000)
		if ((m_Ctrl & 1) != 0) { // DMA mode
			m_BufferAddr = value;
			m_Ctrl |= 2; // BUSY = 1
		} else {	// PIO mode
			m_PioChar = static_cast<uint8_t>(value & 0xFF);
			m_Ctrl |= 2; // BUSY = 1
		}
		m_EventQueue.ScheduleDelta(8, this);
	}
}

uint32_t OutputDevice::OnEvent(uint32_t currentTick)
{
	ProcessDeviceTick(currentTick);
	return ((m_Ctrl & 2) != 0) ? 8 : 0;
}

void OutputDevice::ProcessDeviceTick(uint32_t currentTick)
{
	if ((m_Ctrl & 2) == 0) {
		return;
	}
	if ((m_Ctrl & 1) == 0) { // PIO mode
		char ch = static_cast<char>(m_PioChar);
		std::cout << ch;
		std::cout.flush();
		m_Ctrl &= ~2; // BUSY = 0
		if ((m_Ctrl & 4) != 0) {
			m_BusRouter.WriteByte(0xE024, 2, currentTick);
		}
	} else { // DMA mode
		if (m_Size > 0) {
			char ch = static_cast<char>(m_BusRouter.ReadByte(m_BufferAddr, currentTick));
			std::cout << ch;
			std::cout.flush();
			m_BufferAddr++;
			m_Size--;
			if (m_Size == 0) {
				m_Ctrl &= ~2; // BUSY = 0
				if ((m_Ctrl & 4) != 0) {
					m_BusRouter.WriteByte(0xE024, 2, currentTick);
				}
			}
		} else {
			m_Ctrl &= ~2;
		}
	}
}
}