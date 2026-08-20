#include <gtest/gtest.h>
#include "Devices/OutputDevice.h"
#include "Devices/PicDevice.h"
#include "BusRouter.h"
#include "Ram.h"
#include <sstream>

namespace Emulator::Tests {

// 1. Проверка асинхронного PIO режима: символ выводится только по тику устройства
TEST(OutputDeviceTest, AsyncPioModeOutputsSingleChar)
{
	Ram ram(65536);
	BusRouter bus(ram);
	OutputDevice out(bus);
	std::stringstream buffer;
	std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());
	out.WriteByte(6, 0);   // CTRL = 0 (PIO режим)
	out.WriteWord(0, 'A'); // DATA = 'A'
	// До выполнения тика устройства строка еще не должна быть выведена, а BUSY взведен
	EXPECT_TRUE((out.ReadByte(6) & 2) != 0); // BUSY == 1
	EXPECT_EQ(buffer.str(), "");
	// Выполняем шаг работы устройства
	out.ProcessDeviceTick(8);
	std::cout.rdbuf(oldCout);
	EXPECT_EQ(buffer.str(), "A");
	EXPECT_FALSE((out.ReadByte(6) & 2) != 0); // BUSY сброшен в 0
}
// 2. Проверка пошагового DMA режима из RAM
TEST(OutputDeviceTest, AsyncDmaModeOutputsBufferFromRamStepByStep)
{
	Ram ram(65536);
	BusRouter bus(ram);
	OutputDevice out(bus);
	std::string text = "Hi";
	for (size_t i = 0; i < text.size(); ++i) {
		ram.WriteByte(0x2000 + static_cast<uint32_t>(i), text[i], 1);
	}
	std::stringstream buffer;
	std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());
	out.WriteHalfWord(4, 2);				 // SIZE = 2 байта
	out.WriteByte(6, 1);					 // CTRL = 1 (DMA режим, IE = 0)
	out.WriteWord(0, 0x2000);				 // DATA = 0x2000 (Старт DMA)
	EXPECT_TRUE((out.ReadByte(6) & 2) != 0); // BUSY == 1
	// Шаг 1: выводится первый символ 'H'
	out.ProcessDeviceTick(8);
	EXPECT_EQ(buffer.str(), "H");
	EXPECT_TRUE((out.ReadByte(6) & 2) != 0); // BUSY всё еще 1
	// Шаг 2: выводится второй символ 'i', буфер исчерпан
	out.ProcessDeviceTick(16);
	EXPECT_EQ(buffer.str(), "Hi");
	EXPECT_FALSE((out.ReadByte(6) & 2) != 0); // BUSY сброшен в 0
	std::cout.rdbuf(oldCout);
}
// 3. Проверка генерации прерывания (вектор 2) при завершении DMA вывода
TEST(OutputDeviceTest, GeneratesInterruptOnDmaCompletionWhenEnabled)
{
	Ram ram(65536);
	BusRouter bus(ram);
	PicDevice pic;
	OutputDevice out(bus);
	bus.RegisterDevice(&pic);
	bus.RegisterDevice(&out);
	ram.WriteByte(0x3000, 'X', 1);
	std::stringstream buffer;
	std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());
	out.WriteHalfWord(4, 1);  // SIZE = 1
	out.WriteByte(6, 5);	  // CTRL: DMA = 1 (бит 0), IE = 1 (бит 2) -> 1 | 4 = 5
	out.WriteWord(0, 0x3000); // Старт DMA
	// Шаг вывода: символ напечатан, BUSY перешел из 1 в 0 -> шлется вектор 2 в PIC
	out.ProcessDeviceTick(8);
	std::cout.rdbuf(oldCout);
	EXPECT_TRUE(pic.HasPendingInterrupt());
	EXPECT_EQ(pic.ReadByte(0), 2); // Вектор устройства вывода #2
}


}