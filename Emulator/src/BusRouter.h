#pragma once

#include "IDevice.h"
#include "Ram.h"
#include <array>
#include <stdexcept>

namespace Emulator {
class BusRouter {
public:
	BusRouter(Ram& ram)
		: m_Ram(ram) {}

	Ram& GetRam() { return m_Ram; }

	bool IsValidRange(uint64_t address, uint32_t size) const {
		return CheckValidAccess(static_cast<uint32_t>(address), size);
	}

	void RegisterDevice(IDevice* device);

	uint8_t ReadByte(uint32_t address, uint32_t tick);
	uint16_t ReadHalfword(uint32_t address, uint32_t tick);
	uint32_t ReadWord(uint32_t address, uint32_t tick);

	void WriteByte(uint32_t address, uint8_t value, uint32_t tick);
	void WriteHalfword(uint32_t address, uint16_t value, uint32_t tick);
	void WriteWord(uint32_t address, uint32_t value, uint32_t tick);

	void Tick(uint32_t currentTick);

	void FreeMemoryRegion(uint32_t address, uint32_t size) {
		m_Ram.FreeMemoryRegion(address, size);
	}

private:
	Ram& m_Ram;
	std::array<IDevice*, 3> m_Devices{};

	IDevice* FindDevice(uint32_t address) const;
	bool CheckValidAccess(uint32_t address, uint32_t size) const;
};
}