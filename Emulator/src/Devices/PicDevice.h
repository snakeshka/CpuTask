// Programmable Interrupt Controller
#pragma once

#include "IDevice.h"
#include <cstdint>

namespace Emulator {

class PicDevice : public IDevice {
public:
	static constexpr uint32_t BaseAddr = 0xE020;
	static constexpr uint32_t DeviceSize = 8;

	explicit PicDevice(bool tracePic = false)
		: m_TracePic(tracePic) {}

	uint32_t GetBaseAddress() const override { return BaseAddr; }
	uint32_t GetSize() const override { return DeviceSize; }

	void SetTracePic(bool enabled) { m_TracePic = enabled; }

	uint8_t ReadByte(uint32_t offset) override;
	void WriteByte(uint32_t offset, uint8_t value) override;
	
	// Установки бита прерывания
	void Push(uint8_t vector, uint32_t currentTick);

	// Проверка на нераспределенные прерывания для ядра (MASK != 0 && !PEND)
	bool HasPendingInterrupt() const { return m_Mask != 0 && !m_Pend; }

	void AcknowledgeInterrupt() { m_Pend = true; }

	uint32_t GetMask() const { return m_Mask; }
	bool IsPending() const { return m_Pend; }

private:
	uint32_t m_Mask = 0;
	bool m_Pend = false;	// Флаг запрета повторного вызова прерывания до чтения POP
	bool m_TracePic = false;
	uint32_t m_CurrentTick = 0;
};

}