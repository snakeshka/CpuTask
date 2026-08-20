#include "PicDevice.h"
#include <bit>
#include <iostream>

namespace Emulator {

uint8_t PicDevice::ReadByte(uint32_t offset) {
	// Регистр POP по адресу 0xE020 (offset == 0)
	if (offset == 0) {
		if (m_Mask == 0) {
			return 0;
		}

		// Находим позицию младшего установленного бита в маске
		int pos = std::countr_zero(m_Mask);

		// Сбрасываем бит из маски
		m_Mask &= ~(1 << pos);

		// Разрешаем генерацию следующих внешних прерываний
		m_Pend = false;

		if (m_TracePic) {
			std::cout << m_CurrentTick << " POP " << pos << " MASK=" << m_Mask << '\n';
		}

		return static_cast<uint8_t>(pos);
	}

	return 0;
}

void PicDevice::WriteByte(uint32_t offset, uint8_t value) {
	if (offset == 0) {	// POP
		m_Pend = false;
		return;
	}
	if (offset == 4) { // PUSH
		if (value < 32) {
			m_Mask |= (1u << value);
		}
		if (m_TracePic) {
			std::cout << m_CurrentTick << " PUSH " << static_cast<uint32_t>(value) << " MASK=" << m_Mask << '\n';
		}
	}
}

}