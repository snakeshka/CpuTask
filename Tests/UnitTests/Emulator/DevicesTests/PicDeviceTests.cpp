#include <gtest/gtest.h>
#include "Devices/PicDevice.h"

namespace Emulator::Tests {

TEST(PicDeviceTest, PushAndPopSingleInterrupt) {
	PicDevice pic;

	// Изначально прерываний нет
	EXPECT_FALSE(pic.HasPendingInterrupt());
	EXPECT_EQ(pic.GetMask(), 0);

	// Отправляем вектор прерывания #5
	pic.Push(5, 1);
	EXPECT_EQ(pic.GetMask(), (1u << 5));
	EXPECT_TRUE(pic.HasPendingInterrupt());

	// Квитируем прерывание (вход в обработчик)
	pic.AcknowledgeInterrupt();
	EXPECT_TRUE(pic.IsPending());
	EXPECT_FALSE(pic.HasPendingInterrupt()); // Новые прерывания заблокированы PEND

	// Процессор читает POP (0xE020)
	uint8_t poppedVector = pic.ReadByte(0); // offset 0 = POP
	EXPECT_EQ(poppedVector, 5);
	EXPECT_EQ(pic.GetMask(), 0);
	EXPECT_FALSE(pic.IsPending()); // PEND сбросился
}

TEST(PicDeviceTest, PriorityLowestBitFirst) {
	PicDevice pic;

	// Отправляем вектра #8 и #3
	pic.Push(8, 1);
	pic.Push(3, 1);

	// Сначала должен вычитаться наименьший бит (#3)
	EXPECT_EQ(pic.ReadByte(0), 3);
	// Затем вектор #8
	EXPECT_EQ(pic.ReadByte(0), 8);
	// Далее маска пуста
	EXPECT_EQ(pic.ReadByte(0), 0);
}

}