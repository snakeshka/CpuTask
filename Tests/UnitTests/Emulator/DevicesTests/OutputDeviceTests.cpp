#include <gtest/gtest.h>
#include "Devices/OutputDevice.h"
#include "BusRouter.h"
#include "Ram.h"
#include <sstream>

namespace Emulator::Tests {

TEST(OutputDeviceTest, PioModeOutputsSingleChar) {
	Ram ram(65536);
	BusRouter bus(ram);
	OutputDevice out(bus);

	// Перехватываем std::cout
	std::stringstream buffer;
	std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

	out.WriteByte(6, 0); // CTRL = 0 (PIO mode)
	out.WriteWord(0, 'A'); // DATA = 'A'

	std::cout.rdbuf(oldCout); // Восстанавливаем std::cout

	EXPECT_EQ(buffer.str(), "A");
}

TEST(OutputDeviceTest, DmaModeOutputsBufferFromRam) {
	Ram ram(65536);
	BusRouter bus(ram);
	OutputDevice out(bus);

	// Записываем строку "Hello" в память по адресу 0x2000
	std::string text = "Hello";
	for (size_t i = 0; i < text.size(); ++i) {
		ram.WriteByte(0x2000 + static_cast<uint32_t>(i), text[i], 1);
	}

	std::stringstream buffer;
	std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

	out.WriteHalfWord(4, 5); // SIZE = 5 байт
	out.WriteByte(6, 1);     // CTRL = 1 (DMA mode)
	out.WriteWord(0, 0x2000); // DATA = 0x2000 (запуск DMA)

	std::cout.rdbuf(oldCout);

	EXPECT_EQ(buffer.str(), "Hello");
}

}