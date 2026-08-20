#pragma once

#include "IDevice.h"
#include "BusRouter.h"

namespace Emulator {
class OutputDevice : public IDevice {
public:
	static constexpr uint32_t BaseAddr = 0xE000;
	static constexpr uint32_t DeviceSize = 8;

	OutputDevice(BusRouter& busRouter)
		: m_BusRouter(busRouter) {}

	uint32_t GetBaseAddress() const override { return BaseAddr; }
	uint32_t GetSize() const override { return DeviceSize; }

	uint8_t ReadByte(uint32_t offset) override;
	void WriteByte(uint32_t offset, uint8_t value) override;
	void WriteHalfWord(uint32_t offset, uint16_t value) override;
	void WriteWord(uint32_t offset, uint32_t value) override;

	uint32_t GetEventPeriod() const override { return 8; }
	uint32_t OnEvent(uint32_t currentTick);

	void ProcessDeviceTick(uint32_t currentTick);

private:
	BusRouter& m_BusRouter;
	uint16_t m_Size = 0;
	uint8_t m_Ctrl = 0; // Bit 0: DMA
	uint32_t m_BufferAddr = 0;
	uint8_t m_PioChar = 0;
};
}