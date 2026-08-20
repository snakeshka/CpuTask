#pragma once

#include <cstdint>

namespace Emulator {
class IDevice {
public:
	virtual ~IDevice() = default;

	virtual uint32_t GetBaseAddress() const = 0;
	virtual uint32_t GetSize() const = 0;

	virtual uint8_t ReadByte(uint32_t offset) = 0;
	virtual uint16_t ReadHalfWord(uint32_t offset) {
		return static_cast<uint16_t>(ReadByte(offset)) |
		       (static_cast<uint16_t>(ReadByte(offset + 1)) << 8);
	}
	virtual uint32_t ReadWord(uint32_t offset) {
		return static_cast<uint32_t>(ReadHalfWord(offset)) |
		       (static_cast<uint32_t>(ReadHalfWord(offset + 2)) << 16);
	}

	virtual void WriteByte(uint32_t offset, uint8_t value) = 0;
	virtual void WriteHalfWord(uint32_t offset, uint16_t value) {
		WriteByte(offset, value & 0xFF);
		WriteByte(offset + 1, (value >> 8) & 0xFF);
	}
	virtual void WriteWord(uint32_t offset, uint32_t value) {
		WriteHalfWord(offset, value & 0xFFFF);
		WriteHalfWord(offset + 2, (value >> 16) & 0xFFFF);
	}

	virtual uint32_t GetEventPeriod() const { return 0; }
	virtual void Tick(uint32_t currentTick) { (void)currentTick; }
};
}