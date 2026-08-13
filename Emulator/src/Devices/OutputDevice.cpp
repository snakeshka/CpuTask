#include "OutputDevice.h"
#include <iostream>

namespace Emulator {
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

void OutputDevice::WriteWord(uint32_t offset, uint32_t value) {
	if (offset == 0) { // DATA register (0xE000)
		if ((m_Ctrl & 1) != 0) { // DMA mode
			uint32_t bufferAddr = value;
			for (uint16_t i = 0; i < m_Size; ++i) {
				char ch = static_cast<char>(m_BusRouter.ReadByte(bufferAddr + i, 0));
				std::cout << ch;
			}
			std::cout.flush();
		} else { // PIO mode
			char ch = static_cast<char>(value & 0xFF);
			std::cout << ch;
			std::cout.flush();
		}
	}
}
}