#include "Tracer.h"
#include "CpuState.h"
#include "Cpu/Isa/Instruction.h"
#include "Cpu/Isa/Opcode.h"

#include <gtest/gtest.h>
#include <sstream>
#include <iostream>

namespace
{
// ��������������� ����� ��� ��������� stdout �� ����� ����� (RAII �������)
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

// ��������� ������������ ����������� �������� �������� (ADD)
TEST(TracerTests, TracesAddInstructionCorrectly)
{
	RedirectStdout redirect;

	Emulator::CpuState state;
	state.SetRegister(2, 0xbeef0000); // R2 (������ ��������)
	state.SetRegister(3, 0x0000beef); // R3 (������ ��������)

	// ����������: ADD R1 R2 R3
	// Opcode = 0x02, a = 1, b = 2, c = 3
	Cpu::Isa::Instruction instr{Cpu::Isa::Opcode::ADD, 1, 2, 3};

	Emulator::Tracer tracer(true); // ����������� ��������

	// 1. ��������� ��������� �� ����������
	tracer.SaveStateBefore(state, instr);

	// 2. ���������� ��������� ��������� (����������)
	state.SetRegister(1, 0xbeefbeef); // R1 (��������) �������� ����� ��������

	// 3. ������� ��� �����������
	tracer.TraceStep(state, instr);

	std::string output = redirect.GetOutput();

#if TRACING
	// ���� ������ � ���������� TRACING, ��������� ������ ���������� � ��������
	std::string expected =
		"---- 1 0x0 ----\n"
		"0x03020102 ADD R1 (0xbeefbeef) R2 (0xbeef0000) R3 (0xbeef)\n";
	EXPECT_EQ(output, expected);
#else
	// ���� ����������� ��������� �� ����� ����������, ������ ���� �� ������
	EXPECT_TRUE(output.empty());
#endif
}

// ��������� ������������ ����������� �������� �������� ��������� (LI)
TEST(TracerTests, TracesLiInstructionCorrectly)
{
	RedirectStdout redirect;

	Emulator::CpuState state;
	// ����������: LI R3 0xbeef
	// Opcode = 0x00, a = 3, b = 0xef, c = 0xbe
	Cpu::Isa::Instruction instr{Cpu::Isa::Opcode::LI, 3, 0xef, 0xbe};

	Emulator::Tracer tracer(true);

	tracer.SaveStateBefore(state, instr);
	state.SetRegister(3, 0x0000beef); // ����� �������� � R3
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

// ��������� ���������� ������, ���� ���� --trace=disasm �� ��� �������
TEST(TracerTests, DoesNotTraceIfDisabledAtRuntime)
{
	RedirectStdout redirect;

	Emulator::CpuState state;
	Cpu::Isa::Instruction instr{Cpu::Isa::Opcode::ADD, 1, 2, 3};

	Emulator::Tracer tracer(false); // ��������� �� ����� ����������

	tracer.SaveStateBefore(state, instr);
	tracer.TraceStep(state, instr);

	std::string output = redirect.GetOutput();
	EXPECT_TRUE(output.empty());
}

TEST(TracerTests, FiltersOutTicksOutsideInterval)
{
	RedirectStdout redirect;
	Emulator::CpuState state;
	// ������: ��������� ������ ����� 1, 2 � 5
	std::vector<Emulator::TickInterval> intervals = {{1, 2}, {5, 5}};
	Emulator::Tracer tracer(true, intervals);
	Cpu::Isa::Instruction instr{Cpu::Isa::Opcode::ADD, 1, 2, 3};
	// ���� 1 (������ ���������)
	tracer.SaveStateBefore(state, instr);
	tracer.TraceStep(state, instr);
	// ���� 2 (������ ���������)
	tracer.SaveStateBefore(state, instr);
	tracer.TraceStep(state, instr);
	// ���� 3 (����� �������)
	tracer.SaveStateBefore(state, instr);
	tracer.TraceStep(state, instr);
	// ���� 4 (����� �������)
	tracer.SaveStateBefore(state, instr);
	tracer.TraceStep(state, instr);
	// ���� 5 (������ ���������)
	tracer.SaveStateBefore(state, instr);
	tracer.TraceStep(state, instr);
	std::string output = redirect.GetOutput();
#if TRACING
	// ��������� ������� ������ 1, 2 � 5 � ������ � ���������� 3 � 4
	EXPECT_TRUE(output.find("---- 1 ") != std::string::npos);
	EXPECT_TRUE(output.find("---- 2 ") != std::string::npos);
	EXPECT_FALSE(output.find("---- 3 ") != std::string::npos);
	EXPECT_FALSE(output.find("---- 4 ") != std::string::npos);
	EXPECT_TRUE(output.find("---- 5 ") != std::string::npos);
#else
	EXPECT_TRUE(output.empty());
#endif
}