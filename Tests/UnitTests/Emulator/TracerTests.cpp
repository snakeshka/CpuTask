#include "Tracer.h"
#include "CpuState.h"
#include "Cpu/Isa/Instruction.h"
#include "Cpu/Isa/Opcode.h"

#include <gtest/gtest.h>
#include <sstream>
#include <iostream>

namespace
{
// Вспомогательный класс для перехвата stdout во время теста (RAII паттерн)
class RedirectStdout
{
public:
	RedirectStdout()
	{
		m_OldBuffer = std::cout.rdbuf(m_LocalStream.rdbuf());
	}

	~RedirectStdout()
	{
		std::cout.rdbuf(m_OldBuffer);
	}

	std::string GetOutput() const
	{
		return m_LocalStream.str();
	}

private:
	std::stringstream m_LocalStream;
	std::streambuf *m_OldBuffer;
};

}

// Тестируем корректность трассировки операции сложения (ADD)
TEST(TracerTests, TracesAddInstructionCorrectly)
{
	RedirectStdout redirect;

	Emulator::CpuState state;
	state.SetRegister(2, 0xbeef0000); // R2 (первый источник)
	state.SetRegister(3, 0x0000beef); // R3 (второй источник)

	// Инструкция: ADD R1 R2 R3
	// Opcode = 0x02, a = 1, b = 2, c = 3
	Cpu::Isa::Instruction instr{Cpu::Isa::Opcode::ADD, 1, 2, 3};

	Emulator::Tracer tracer(true); // Трассировка включена

	// 1. Сохраняем состояние ДО выполнения
	tracer.SaveStateBefore(state, instr);

	// 2. Симулируем изменение состояния (выполнение)
	state.SetRegister(1, 0xbeefbeef); // R1 (приемник) получает новое значение

	// 3. Выводим шаг трассировки
	tracer.TraceStep(state, instr);

	std::string output = redirect.GetOutput();

#if TRACING
	// Если сборка с включенным TRACING, проверяем точное совпадение с эталоном
	std::string expected =
		"---- 1 0x0 ----\n"
		"0x03020102 ADD R1 (0xbeefbeef) R2 (0xbeef0000) R3 (0xbeef)\n";
	EXPECT_EQ(output, expected);
#else
	// Если трассировка отключена на этапе компиляции, вывода быть не должно
	EXPECT_TRUE(output.empty());
#endif
}

// Тестируем корректность трассировки операции загрузки константы (LI)
TEST(TracerTests, TracesLiInstructionCorrectly)
{
	RedirectStdout redirect;

	Emulator::CpuState state;
	// Инструкция: LI R3 0xbeef
	// Opcode = 0x00, a = 3, b = 0xef, c = 0xbe
	Cpu::Isa::Instruction instr{Cpu::Isa::Opcode::LI, 3, 0xef, 0xbe};

	Emulator::Tracer tracer(true);

	tracer.SaveStateBefore(state, instr);
	state.SetRegister(3, 0x0000beef); // Новое значение в R3
	tracer.TraceStep(state, instr);

	std::string output = redirect.GetOutput();

#if TRACING
	std::string expected =
		"---- 1 0x0 ----\n"
		"0xbeef0300 LI R3 (0xbeef) imm (0xbeef)\n";
	EXPECT_EQ(output, expected);
#else
	EXPECT_TRUE(output.empty());
#endif
}

// Тестируем отключение вывода, если флаг --trace=disasm не был передан
TEST(TracerTests, DoesNotTraceIfDisabledAtRuntime)
{
	RedirectStdout redirect;

	Emulator::CpuState state;
	Cpu::Isa::Instruction instr{Cpu::Isa::Opcode::ADD, 1, 2, 3};

	Emulator::Tracer tracer(false); // Выключено на этапе выполнения

	tracer.SaveStateBefore(state, instr);
	tracer.TraceStep(state, instr);

	std::string output = redirect.GetOutput();
	EXPECT_TRUE(output.empty());
}