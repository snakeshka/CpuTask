#include "Emulator.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <charconv>
#include <iterator>
#include <optional>

std::vector<Emulator::TickInterval> parseTraceTicks(std::string_view value)
{
	std::vector<Emulator::TickInterval> rawIntervals;

	size_t start = 0;
	while (start < value.size())
	{
		size_t comma = value.find(',', start);
		std::string_view part = value.substr(start, comma - start);

		size_t dash = part.find('-');
		if (dash == std::string_view::npos)
		{
			// Одиночный такт
			unsigned int tick = 0;
			auto res = std::from_chars(part.data(), part.data() + part.size(), tick);
			if (res.ec == std::errc{})
			{
				rawIntervals.push_back({tick, tick});
			}
		}
		else
		{
			// Диапазон тактов
			std::string_view startPart = part.substr(0, dash);
			std::string_view endPart = part.substr(dash + 1);

			unsigned int start_tick = 0;
			unsigned int end_tick = 0;

			auto res1 = std::from_chars(startPart.data(), startPart.data() + startPart.size(), start_tick);
			auto res2 = std::from_chars(endPart.data(), endPart.data() + endPart.size(), end_tick);

			if (res1.ec == std::errc{} && res2.ec == std::errc{})
			{
				rawIntervals.push_back({start_tick, end_tick});
			}
		}

		if (comma == std::string_view::npos)
			break;
		start = comma + 1;
	}
	if (rawIntervals.empty())
		return {};
	// Сортируем интервалы по началу диапазона
	std::ranges::sort(rawIntervals.begin(), rawIntervals.end(), [](const Emulator::TickInterval &a, const Emulator::TickInterval &b)
			  { return a.begin < b.begin; });
	// Сливаем перекрывающиеся интервалы
	std::vector<Emulator::TickInterval> mergedIntervals;
	mergedIntervals.push_back(rawIntervals[0]);
	for (size_t i = 1; i < rawIntervals.size(); ++i)
	{
		auto &last = mergedIntervals.back();
		const auto &current = rawIntervals[i];
		if (current.begin <= last.end)
		{
			last.end = std::max(last.end, current.end); // Склеиваем
		}
		else
		{
			mergedIntervals.push_back(current);
		}
	}
	return mergedIntervals;
}

std::vector<Emulator::AddrInterval> parseAddrIntervals(std::string_view value)
{
	std::vector<Emulator::AddrInterval> rawIntervals;
	size_t start = 0;
	while (start < value.size())
	{
		size_t comma = value.find(',', start);
		std::string_view part = value.substr(start, comma - start);
		size_t dash = part.find('-');
		if (dash == std::string_view::npos)
		{
			// Одиночный адрес
			unsigned int addr = 0;
			int base = 10;
			if (part.starts_with("0x") || part.starts_with("0X"))
			{
				part.remove_prefix(2);
				base = 16;
			}
			auto res = std::from_chars(part.data(), part.data() + part.size(), addr, base);
			if (res.ec == std::errc{})
			{
				rawIntervals.push_back({addr, addr});
			}
		}
		else
		{
			// Диапазон адресов
			std::string_view startPart = part.substr(0, dash);
			std::string_view endPart = part.substr(dash + 1);
			unsigned int startAddr = 0;
			unsigned int endAddr = 0;
			int startBase = 10;
			if (startPart.starts_with("0x") || startPart.starts_with("0X"))
			{
				startPart.remove_prefix(2);
				startBase = 16;
			}
			int end_base = 10;
			if (endPart.starts_with("0x") || endPart.starts_with("0X"))
			{
				endPart.remove_prefix(2);
				end_base = 16;
			}
			auto res1 = std::from_chars(startPart.data(), startPart.data() + startPart.size(), startAddr, startBase);
			auto res2 = std::from_chars(endPart.data(), endPart.data() + endPart.size(), endAddr, end_base);
			if (res1.ec == std::errc{} && res2.ec == std::errc{})
			{
				rawIntervals.push_back({startAddr, endAddr});
			}
		}
		if (comma == std::string_view::npos)
			break;
		start = comma + 1;
	}
	if (rawIntervals.empty())
		return {};
	// Сортируем интервалы по возрастанию адреса
	std::ranges::sort(rawIntervals.begin(), rawIntervals.end(), [](const Emulator::AddrInterval &a, const Emulator::AddrInterval &b)
			  { return a.begin < b.begin; });
	// Склеиваем пересекающиеся или прилегающие интервалы
	std::vector<Emulator::AddrInterval> mergedIntervals;
	mergedIntervals.push_back(rawIntervals[0]);
	for (size_t i = 1; i < rawIntervals.size(); ++i)
	{
		auto &last = mergedIntervals.back();
		const auto &current = rawIntervals[i];
		if (current.begin <= last.end)
		{
			last.end = std::max(last.end, current.end); // Расширяем интервал
		}
		else
		{
			mergedIntervals.push_back(current);
		}
	}
	return mergedIntervals;
}

namespace
{
std::optional<Cpu::Bytecode> readBytecode(const std::string &path)
{
	std::ifstream input(path, std::ios::binary);
	if (!input)
	{
		return std::nullopt;
	}

	return Cpu::Bytecode(
		std::istreambuf_iterator<char>(input),
		std::istreambuf_iterator<char>());
}

}

int main(int argc, char *argv[])
{
	std::string programPath;
	bool traceEnabled = false;
	std::vector<Emulator::TickInterval> traceIntervals;
	uint32_t ramSize = 65536;
	bool warnUninit = false;
	bool ramTraceEnabled = false;
	std::vector<Emulator::AddrInterval> ramIntervals;
	for (int i = 1; i < argc; ++i)
	{
		std::string_view arg = argv[i];
		if (arg.starts_with("--ram-size="))
		{
			std::string_view size_str = arg.substr(11);
			uint32_t parsed_size = 0;
			std::from_chars(size_str.data(), size_str.data() + size_str.size(), parsed_size);
			ramSize = parsed_size;
		}
		else if (arg == "--warn=uninit-ram")
		{
			warnUninit = true;
		}
		else if (arg.starts_with("--trace="))
		{
			std::string_view sub = arg.substr(8);
			if (sub.find("ram-wr") != std::string_view::npos)
			{
				ramTraceEnabled = true;
			}
			if (sub.find("disasm") != std::string_view::npos)
			{
				traceEnabled = true;
			}
		}
		else if (arg.starts_with("--trace-ram-addrs="))
		{
			ramTraceEnabled = true;
			ramIntervals = parseAddrIntervals(arg.substr(18));
		}
		else if (arg.starts_with("--trace-ticks="))
		{
			traceEnabled = true;
			traceIntervals = parseTraceTicks(arg.substr(14));
		}
		else
		{
			programPath = arg;
		}
	}
	if (programPath.empty())
	{
		std::cerr << "usage: Emulator program.o [--trace=disasm]\n";
		return 1;
	}
	const auto bytecode = readBytecode(programPath);
	if (!bytecode.has_value())
	{
		std::cerr << programPath << ": error: cannot open input file\n";
		return 1;
	}

	Emulator::Emulator emulator;
	const auto result = emulator.Run(
		*bytecode,
		traceEnabled,
		traceIntervals,
		ramSize,
		warnUninit,
		ramIntervals,
		ramTraceEnabled);
	if (!result.Success)
	{
		std::cerr << "error: " << result.Error << '\n';
		return 1;
	}

	for (size_t index = 0; index < 16; ++index)
	{
		std::cout << 'R' << index << "=0x"
				  << std::hex << std::uppercase << std::setw(8) << std::setfill('0')
				  << result.State.GetRegister(index) << std::dec << '\n';
	}

	return 0;
}
