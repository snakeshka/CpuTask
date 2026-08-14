#include "TimerDevice.h"

namespace Emulator {
uint8_t TimerDevice::ReadByte(uint32_t offset) {
	if (offset == 4) { // CTRL register (0xE014)
		return m_Ctrl;
	}
	return 0;
}

uint32_t TimerDevice::ReadWord(uint32_t offset) {
	if (offset == 0) { // CNTD register (0xE010)
		return m_Cntd;
	}
	return 0;
}

void TimerDevice::WriteByte(uint32_t offset, uint8_t value) {
	if (offset == 4) { // CTRL register
		m_Ctrl = value;
	}
}

void TimerDevice::WriteWord(uint32_t offset, uint32_t value) {
	if (offset == 0) { // CNTD register
		m_Cntd = value;
	}
}
}