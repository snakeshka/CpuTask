#include <gtest/gtest.h>
#include "Devices/TimerDevice.h"
#include "Devices/PicDevice.h"

namespace Emulator::Tests {

TEST(TimerDeviceTest, CountdownAndTrigFlag) {
	Emulator::Ram ram(65536);
	Emulator::BusRouter bus(ram);
	TimerDevice timer(bus);

	// Устанавливаем CNTD = 3
	timer.WriteWord(0, 3); // offset 0 = CNTD
	EXPECT_EQ(timer.ReadWord(0), 3);

	// CTRL = 0 (TRIG=0, IE=0)
	timer.WriteByte(4, 0); // offset 4 = CTRL

	timer.Tick(1); // CNTD -> 2
	EXPECT_EQ(timer.ReadWord(0), 2);
	EXPECT_EQ(timer.ReadByte(4) & 1, 0); // TRIG == 0

	timer.Tick(2); // CNTD -> 1
	timer.Tick(3); // CNTD -> 0 -> TRIG = 1

	EXPECT_EQ(timer.ReadWord(0), 0);
	EXPECT_EQ(timer.ReadByte(4) & 1, 1); // TRIG == 1
}

// 2. Проверка отправки прерывания в PIC через BusRouter при IE == 1
TEST(TimerDeviceTest, SendsMsiInterruptToPicWhenEnabled)
{
	Emulator::Ram ram(65536);
	Emulator::BusRouter bus(ram);
	PicDevice pic;
	TimerDevice timer(bus);
	bus.RegisterDevice(&pic);
	bus.RegisterDevice(&timer);
	timer.WriteWord(0, 2); // CNTD = 2
	timer.WriteByte(4, 2); // CTRL: IE = 1 (бит 1), TRIG = 0
	timer.Tick(1);
	EXPECT_FALSE(pic.HasPendingInterrupt());
	timer.Tick(2); // CNTD дошел до 0 -> таймер пишет в 0xE024 (PIC PUSH)
	EXPECT_TRUE(pic.HasPendingInterrupt());
	EXPECT_EQ(pic.ReadByte(0), 1); // Вектор таймера #1
}

}