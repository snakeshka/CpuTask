#include "Ram.h"
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include "Cpu/Common/Assert.h"

namespace Emulator
{
Ram::Ram(uint32_t maxSize, bool warnUninit)
	: m_MaxSize(maxSize)
	, m_WarnUninit(warnUninit)
{
	if (maxSize % 4 != 0)
	{
		throw std::runtime_error("RAM size must be a multiple of 4 bytes!");
	}
}

void Ram::CheckBounds(uint32_t address) const
{
	if (address >= m_MaxSize)
	{
		throw std::out_of_range("Internal error: unchecked RAM access at address 0x" + std::to_string(address));
	}
}

bool Ram::IsValidRange(std::uint64_t address, uint32_t size) const
{
	return (address + size <= m_MaxSize);
}

Ram::Page &Ram::GetOrCreatePage(uint32_t pageIndex)
{
	auto &pagePtr = m_Pages[pageIndex];
	if (!pagePtr)
	{
		pagePtr = std::make_unique<Page>();
	}
	return *pagePtr;
}

void Ram::WriteByteInternal(uint32_t address, uint8_t value)
{
	uint32_t pageIndex = address / PageSize;
	uint32_t offset = address % PageSize;
	Page &page = GetOrCreatePage(pageIndex);
	page.data[offset] = value;
	page.initialized[offset] = true;
}

uint8_t Ram::ReadByteInternal(uint32_t address, uint32_t tick)
{
	uint32_t pageIndex = address / PageSize;
	uint32_t offset = address % PageSize;

	const auto it = m_Pages.find(pageIndex);
	if (it == m_Pages.end())
	{
		if (m_WarnUninit)
		{
			std::cerr << "[WARN] Tick " << std::dec << tick << ": Read uninitialized byte at RAM address 0x"
					  << std::hex << address << "\n";
		}
		return 0;
	}

	Page &page = *it->second;
	if (m_WarnUninit && !page.initialized[offset])
	{
		std::cerr << "[WARN] Tick " << std::dec << tick << ": Read uninitialized byte at RAM address 0x"
				  << std::hex << address << "\n";
	}

	return page.data[offset];
}

uint8_t Ram::ReadByte(uint32_t address, uint32_t tick)
{
	CheckBounds(address);
	return ReadByteInternal(address, tick);
}

uint16_t Ram::ReadHalfword(uint32_t address, uint32_t tick)
{
	CheckBounds(address + 1);
	return static_cast<uint16_t>(ReadByteInternal(address, tick)) |
		(static_cast<uint16_t>(ReadByteInternal(address + 1, tick)) << 8);
}

uint32_t Ram::ReadWord(uint32_t address, uint32_t tick)
{
	CheckBounds(address + 3);
	uint32_t val = 0;
	for (uint32_t i = 0; i < 4; ++i)
	{
		val |= (static_cast<uint32_t>(ReadByteInternal(address + i, tick)) << (i * 8));
	}
	return val;
}

void Ram::LogTrace(uint32_t cellAddr, uint32_t oldVal, uint32_t newVal, uint32_t tick) const
{
	if (tick != 0 && m_TraceEnabled && IsAddrTraceAllowed(cellAddr))
	{
		std::cout << std::dec << tick << " RAM [0x" << std::hex << std::nouppercase << std::setfill('0') << std::setw(8) << cellAddr
				  << "] wr 0x" << std::setw(8) << oldVal << " -> 0x" << std::setw(8) << newVal << "\n";
	}
}

void Ram::WriteByte(uint32_t address, uint8_t value, uint32_t tick)
{
	CheckBounds(address);
	uint32_t cellAddr = address & ~3;
	uint32_t oldVal = ReadWord(cellAddr, tick);

	WriteByteInternal(address, value);

	uint32_t newVal = ReadWord(cellAddr, tick);
	LogTrace(cellAddr, oldVal, newVal, tick);
}

void Ram::WriteHalfword(uint32_t address, uint16_t value, uint32_t tick)
{
	CheckBounds(address + 1);

	uint32_t cell0Addr = address & ~3;
	uint32_t cell1Addr = (address + 1) & ~3;

	uint32_t oldCell0 = ReadWord(cell0Addr, tick);
	uint32_t oldCell1 = (cell0Addr != cell1Addr) ? ReadWord(cell1Addr, tick) : 0;

	WriteByteInternal(address, static_cast<uint8_t>(value & 0xFF));
	WriteByteInternal(address + 1, static_cast<uint8_t>((value >> 8) & 0xFF));

	uint32_t newCell0 = ReadWord(cell0Addr, tick);
	uint32_t newCell1 = (cell0Addr != cell1Addr) ? ReadWord(cell1Addr, tick) : 0;

	LogTrace(cell0Addr, oldCell0, newCell0, tick);
	if (cell0Addr != cell1Addr)
	{
		LogTrace(cell1Addr, oldCell1, newCell1, tick);
	}
}

void Ram::WriteWord(uint32_t address, uint32_t value, uint32_t tick)
{
	CheckBounds(address + 3);

	uint32_t cell0Addr = address & ~3;
	uint32_t cell1Addr = (address + 3) & ~3;

	uint32_t oldCell0 = ReadWord(cell0Addr, tick);
	uint32_t oldCell1 = (cell0Addr != cell1Addr) ? ReadWord(cell1Addr, tick) : 0;

	for (uint32_t i = 0; i < 4; ++i)
	{
		WriteByteInternal(address + i, static_cast<uint8_t>((value >> (i * 8)) & 0xFF));
	}

	uint32_t newCell0 = ReadWord(cell0Addr, tick);
	uint32_t newCell1 = (cell0Addr != cell1Addr) ? ReadWord(cell1Addr, tick) : 0;

	LogTrace(cell0Addr, oldCell0, newCell0, tick);
	if (cell0Addr != cell1Addr)
	{
		LogTrace(cell1Addr, oldCell1, newCell1, tick);
	}
}

bool Ram::IsAddrTraceAllowed(uint32_t address) const
{
	if (m_TraceIntervals.empty())
		return true;
	for (const auto &interval : m_TraceIntervals)
	{
		if (address >= interval.begin && address <= interval.end)
			return true;
	}
	return false;
}

void Ram::SetTraceEnabled(bool enabled, std::vector<AddrInterval> intervals)
{
	m_TraceEnabled = enabled;
	m_TraceIntervals = std::move(intervals);
}

}