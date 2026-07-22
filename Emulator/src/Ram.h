#pragma once

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>

namespace Emulator
{
struct AddrInterval
{
	uint32_t begin;
	uint32_t end;
};

class Ram
{
public:
	static constexpr uint32_t PageSize = 4096;

	Ram(uint32_t maxSize, bool warnUninit = false);

	bool IsValidRange(std::uint64_t address, uint32_t size) const;
	void SetTraceEnabled(bool enabled, std::vector<AddrInterval> intervals = {});
	void FreeMemoryRegion(uint32_t address, uint32_t size);
	void ResetInitFlagInternal(uint32_t address);

	// Публичные методы чтения\записи памяти естественного размера
	uint8_t ReadByte(uint32_t address, uint32_t tick);
	uint16_t ReadHalfword(uint32_t address, uint32_t tick);
	uint32_t ReadWord(uint32_t address, uint32_t tick);

	void WriteByte(uint32_t address, uint8_t value, uint32_t tick);
	void WriteHalfword(uint32_t address, uint16_t value, uint32_t tick);
	void WriteWord(uint32_t address, uint32_t value, uint32_t tick);

private:
	struct Page
	{
		uint8_t data[PageSize]{};
		bool initialized[PageSize]{};
	};

	uint32_t m_MaxSize = 0;
	bool m_WarnUninit = false;
	bool m_TraceEnabled = false;
	bool m_clearStack = false;
	std::vector<AddrInterval> m_TraceIntervals;

	std::unordered_map<uint32_t, std::unique_ptr<Page>> m_Pages;

	Page &GetOrCreatePage(uint32_t pageIndex);
	void CheckBounds(uint32_t address) const;
	bool IsAddrTraceAllowed(uint32_t address) const;

	// Внутреннее чтение/запись байт без проверки границ
	uint8_t ReadByteInternal(uint32_t address, uint32_t tick);
	void WriteByteInternal(uint32_t address, uint8_t value);

	void LogTrace(uint32_t cellAddr, uint32_t oldVal, uint32_t newVal, uint32_t tick) const;
};

}