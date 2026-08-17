#pragma once

#include "IDevice.h"
#include <functional>

namespace Emulator {
class TimerDevice : public IDevice {
public:
	using InterruptCallback = std::function<void(uint8_t vector, uint32_t currentTick)>;

	static constexpr uint32_t BaseAddr = 0xE010;
	static constexpr uint32_t DeviceSize = 8;

	TimerDevice() = default;

	void SetInterruptCallback(InterruptCallback callback) {
		m_InterruptCallback = std::move(callback);
	}

	uint32_t GetBaseAddress() const override { return BaseAddr; }
	uint32_t GetSize() const override { return DeviceSize; }

	uint8_t ReadByte(uint32_t offset) override;
	uint32_t ReadWord(uint32_t offset) override;
	void WriteByte(uint32_t offset, uint8_t value) override;
	void WriteWord(uint32_t offset, uint32_t value) override;

	void Tick(uint32_t currentTick) override {
		static constexpr uint8_t CtrlTrigMask = 0x01;
		static constexpr uint8_t CtrlIeMask = 0x02;
		static constexpr uint8_t TimerIrqVector = 1;
		if ((m_Ctrl & CtrlTrigMask) == 0 && m_Cntd > 0) { // TRIG == 0
			m_Cntd--;
			if (m_Cntd == 0) {
				m_Ctrl |= CtrlTrigMask; // TRIG = 1
				if ((m_Ctrl & CtrlIeMask) != 0 && m_InterruptCallback) { // IE == 1
					m_InterruptCallback(1, currentTick); // Запись вектор ID 1 в PIC
				}
			}
		}
	}

private:
	uint32_t m_Cntd = 0;
	uint8_t m_Ctrl = 0;
	InterruptCallback m_InterruptCallback;
};
}