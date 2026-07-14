// Tests/UnitTests/Emulator/RamTests.cpp
#include "Ram.h"
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>

// Вспомогательный класс для перехвата сообщений из std::cerr
class RedirectStderr
{
public:
	RedirectStderr()
		: m_OldBuffer(std::cerr.rdbuf(m_Buffer.rdbuf()))
	{
	}

	~RedirectStderr()
	{
		std::cerr.rdbuf(m_OldBuffer);
	}

	std::string GetOutput() const
	{
		return m_Buffer.str();
	}

private:
	std::stringstream m_Buffer;
	std::streambuf *m_OldBuffer;
};

// 1. Проверка чтения по умолчанию (неинициализированная память возвращает 0)
TEST(RamTests, InitialReadsReturnZero)
{
	Emulator::Ram ram(1024);

	EXPECT_EQ(ram.ReadByte(0, 1), 0);
	EXPECT_EQ(ram.ReadByte(500, 1), 0);
}

// 2. Проверка корректности записи и последующего чтения
TEST(RamTests, WritesAndReadsCorrectly)
{
	Emulator::Ram ram(1024);

	ram.WriteByte(100, 42, 1);
	ram.WriteByte(101, 255, 1);

	EXPECT_EQ(ram.ReadByte(100, 1), 42);
	EXPECT_EQ(ram.ReadByte(101, 1), 255);
}

// 3. Проверка, что некорректный размер RAM (не кратный 4) вызывает ошибку при создании
TEST(RamTests, InvalidSizeThrowsOnCreation)
{
	EXPECT_THROW(Emulator::Ram ram(1001), std::runtime_error);
}

// 4. Проверка жесткого падения по CheckBounds (для багов самого эмулятора)
TEST(RamTests, OutOfBoundsAccessThrows)
{
	Emulator::Ram ram(1024);

	EXPECT_THROW(ram.ReadByte(1024, 1), std::out_of_range);
	EXPECT_THROW(ram.WriteByte(1025, 42, 1), std::out_of_range);
}

// 5. Проверка разреженной памяти (Sparse Memory)
// Создаем эмуляцию RAM объемом 1 ГБ. Обычный массив вызвал бы Out Of Memory (OOM)
// или зависание, а разреженная память работает мгновенно.
TEST(RamTests, SparseMemoryAllocatesOnDemand)
{
	Emulator::Ram ram(1024 * 1024 * 1024);

	ram.WriteByte(500'000'000, 99, 1);
	EXPECT_EQ(ram.ReadByte(500'000'000, 1), 99);
	EXPECT_EQ(ram.ReadByte(500'000'001, 1), 0);
}

// 6. Проверка генерации предупреждений о чтении неинициализированных данных
TEST(RamTests, WarnsOnUninitializedRead)
{
	Emulator::Ram ram(1024, true); // warn_uninit = true

	RedirectStderr redirect;
	ram.ReadByte(256, 42); // Читаем адрес 0x100 на такте 42

	std::string output = redirect.GetOutput();
	EXPECT_TRUE(output.find("Read uninitialized byte at RAM address 0x100") != std::string::npos);
	EXPECT_TRUE(output.find("Tick 42") != std::string::npos);
}

// 7. Проверка функции IsValidRange для безопасного контроля границ в Executor
TEST(RamTests, IsValidRangeChecksCorrectly)
{
	Emulator::Ram ram(1024);

	EXPECT_TRUE(ram.IsValidRange(0, 4));
	EXPECT_TRUE(ram.IsValidRange(1020, 4));
	EXPECT_FALSE(ram.IsValidRange(1021, 4)); // Заходит на адрес 1025 (выход за границу)
	EXPECT_FALSE(ram.IsValidRange(1024, 1));
}