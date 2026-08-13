#include <gtest/gtest.h>
#include "BusRouter.h"
#include "Ram.h"
#include "IDevice.h"

namespace Emulator::Tests {

// Фейковое устройство для тестирования маршрутизации
class DummyDevice : public IDevice {
public:
	static constexpr uint32_t BaseAddr = 0xE000;
	static constexpr uint32_t Size = 8;

	uint32_t GetBaseAddress() const override { return BaseAddr; }
	uint32_t GetSize() const override { return Size; }

	uint8_t ReadByte(uint32_t offset) override {
		return m_Data[offset];
	}
	void WriteByte(uint32_t offset, uint8_t value) override {
		m_Data[offset] = value;
	}

private:
	uint8_t m_Data[8]{};
};

TEST(BusRouterTest, RedirectsToRamWhenUnmapped) {
	Ram ram(65536);
	BusRouter bus(ram);

	bus.WriteWord(0x1000, 0xDEADBEEF, 1);
	EXPECT_EQ(bus.ReadWord(0x1000, 1), 0xDEADBEEF);
}

TEST(BusRouterTest, RedirectsToRegisteredDevice) {
	Ram ram(65536);
	BusRouter bus(ram);
	DummyDevice dev;

	bus.RegisterDevice(&dev);

	// Запись по MMIO адресу 0xE002 должна попасть в устройство по смещению 2
	bus.WriteByte(0xE002, 0x42, 1);
	EXPECT_EQ(bus.ReadByte(0xE002, 1), 0x42);
	
	// ОЗУ по этому адресу меняться не должна
	EXPECT_EQ(ram.ReadByte(0xE002, 1), 0);
}

TEST(BusRouterTest, ThrowsOnCrossBoundaryAccess) {
	Ram ram(65536);
	BusRouter bus(ram);
	DummyDevice dev; // 0xE000 - 0xE007

	bus.RegisterDevice(&dev);

	// Чтение слова (4 байта) по адресу 0xE006 захватит 0xE006, 0xE007 (устройство) и 0xE008, 0xE009 (RAM)
	// Должно выбросить исключение из-за пересечения границы устройства
	EXPECT_THROW(bus.ReadWord(0xE006, 1), std::runtime_error);
}

}