#include "BusRouter.h"

namespace Emulator {
IDevice* BusRouter::FindDevice(uint32_t address) const {
	for (auto* dev : m_Devices) {
		if (address >= dev->GetBaseAddress() && address < dev->GetBaseAddress() + dev->GetSize()) {
			return dev;
		}
	}
	return nullptr;
}

bool BusRouter::CheckValidAccess(uint32_t address, uint32_t size) const {
	IDevice* startDev = FindDevice(address);
	if (size > 1) {
		IDevice* endDev = FindDevice(address + size - 1);
		if (startDev != endDev) {
			// Пересечение границ между устройствами или памятью
			return false;
		}
	}
	if (startDev == nullptr) {
		return m_Ram.IsValidRange(address, size);
	}
	return true;
}

uint8_t BusRouter::ReadByte(uint32_t address, uint32_t tick) {
	IDevice* dev = FindDevice(address);
	if (dev) {
		return dev->ReadByte(address - dev->GetBaseAddress());
	}
	return m_Ram.ReadByte(address, tick);
}

uint16_t BusRouter::ReadHalfword(uint32_t address, uint32_t tick) {
	if (!CheckValidAccess(address, 2)) {
		throw std::runtime_error("Unaligned or cross-boundary memory access");
	}
	IDevice* dev = FindDevice(address);
	if (dev) {
		return dev->ReadHalfWord(address - dev->GetBaseAddress());
	}
	return m_Ram.ReadHalfword(address, tick);
}

uint32_t BusRouter::ReadWord(uint32_t address, uint32_t tick) {
	if (!CheckValidAccess(address, 4)) {
		throw std::runtime_error("Unaligned or cross-boundary memory access");
	}
	IDevice* dev = FindDevice(address);
	if (dev) {
		return dev->ReadWord(address - dev->GetBaseAddress());
	}
	return m_Ram.ReadWord(address, tick);
}

void BusRouter::WriteByte(uint32_t address, uint8_t value, uint32_t tick) {
	IDevice* dev = FindDevice(address);
	if (dev) {
		dev->WriteByte(address - dev->GetBaseAddress(), value);
		return;
	}
	m_Ram.WriteByte(address, value, tick);
}

void BusRouter::WriteHalfword(uint32_t address, uint16_t value, uint32_t tick) {
	if (!CheckValidAccess(address, 2)) {
		throw std::runtime_error("Unaligned or cross-boundary memory access");
	}
	IDevice* dev = FindDevice(address);
	if (dev) {
		dev->WriteHalfWord(address - dev->GetBaseAddress(), value);
		return;
	}
	m_Ram.WriteHalfword(address, value, tick);
}

void BusRouter::WriteWord(uint32_t address, uint32_t value, uint32_t tick) {
	if (!CheckValidAccess(address, 4)) {
		throw std::runtime_error("Unaligned or cross-boundary memory access");
	}
	IDevice* dev = FindDevice(address);
	if (dev) {
		dev->WriteWord(address - dev->GetBaseAddress(), value);
		return;
	}
	m_Ram.WriteWord(address, value, tick);
}

void BusRouter::Tick(uint32_t currentTick) {
	for (auto* dev : m_Devices) {
		dev->Tick(currentTick);
	}
}
}