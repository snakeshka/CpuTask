#include <gtest/gtest.h>
#include "Devices/PicDevice.h"

namespace Emulator::Tests {

TEST(PicDeviceTest, PushViaWriteByteAndPopViaReadByte)
{
	PicDevice pic;
	// Изначально прерываний нет
	EXPECT_FALSE(pic.HasPendingInterrupt());
	EXPECT_EQ(pic.GetMask(), 0u);
	// Отправляем вектор прерывания #5 через запись в регистр PUSH (offset == 4)
	pic.WriteByte(4, 5);
	EXPECT_EQ(pic.GetMask(), (1u << 5));
	EXPECT_TRUE(pic.HasPendingInterrupt());
	// Квитируем прерывание ядром (вход в обработчик)
	pic.AcknowledgeInterrupt();
	EXPECT_TRUE(pic.IsPending());
	EXPECT_FALSE(pic.HasPendingInterrupt()); // Новые прерывания заблокированы флагом PEND
	// Процессор считывает вектор из регистра POP (offset == 0)
	uint8_t poppedVector = pic.ReadByte(0);
	EXPECT_EQ(poppedVector, 5);
	EXPECT_EQ(pic.GetMask(), 0u);
	// Запись в POP (offset == 0) сбрасывает PEND
	pic.WriteByte(0, 0);
	EXPECT_FALSE(pic.IsPending());
}
// 2. Проверка приоритета прерываний: сначала отдается наименьший номер бита
TEST(PicDeviceTest, PriorityLowestBitFirst)
{
	PicDevice pic;
	// Записываем векторы #8 и #3 в регистр PUSH
	pic.WriteByte(4, 8);
	pic.WriteByte(4, 3);
	// Первым должен вычитаться наименьший бит (#3)
	EXPECT_EQ(pic.ReadByte(0), 3);
	// Затем вектор #8
	EXPECT_EQ(pic.ReadByte(0), 8);
	// Если маска пуста, возвращается 0
	EXPECT_EQ(pic.ReadByte(0), 0);
}
// 3. Проверка игнорирования невалидных векторов (>= 32)
TEST(PicDeviceTest, IgnoresOutOfRangeVectors)
{
	PicDevice pic;
	pic.WriteByte(4, 35); // Вектор больше 31
	EXPECT_EQ(pic.GetMask(), 0u);
	EXPECT_FALSE(pic.HasPendingInterrupt());
}

}