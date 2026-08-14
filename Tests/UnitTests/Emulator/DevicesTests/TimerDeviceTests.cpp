#include <gtest/gtest.h>
#include "Devices/TimerDevice.h"

namespace Emulator::Tests {

TEST(TimerDeviceTest, CountdownAndTrigFlag) {
	TimerDevice timer;

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

TEST(TimerDeviceTest, FiresCallbackWhenInterruptEnabled) {
	TimerDevice timer;
	bool callbackFired = false;
	uint8_t firedVector = 0;

	timer.SetInterruptCallback([&](uint8_t vector, uint32_t tick) {
		callbackFired = true;
		firedVector = vector;
	});

	timer.WriteWord(0, 2); // CNTD = 2
	timer.WriteByte(4, 2); // IE = 1 (bit 1), TRIG = 0 (bit 0)

	timer.Tick(1);
	EXPECT_FALSE(callbackFired);

	timer.Tick(2); // CNTD дошел до 0
	EXPECT_TRUE(callbackFired);
	EXPECT_EQ(firedVector, 1); // Вектор таймера #1
}

}