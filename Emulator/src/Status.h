#pragma once

#include <string_view>

namespace Emulator {
enum class ExecutionStatus
{
	Success,
	DivisionByZero,
	IllegalInstruction,
	MemoryOutOfBounds,
	StackOverflow,
	InvalidScrIndex
};

inline std::string_view ToString(ExecutionStatus status)
{
	switch (status) {
		case ExecutionStatus::Success:
			return "Success";
		case ExecutionStatus::DivisionByZero:
			return "division by zero";
		case ExecutionStatus::IllegalInstruction:
			return "illegal instruction";
		case ExecutionStatus::MemoryOutOfBounds:
			return "memory access out of bounds";
		case ExecutionStatus::StackOverflow:
			return "stack overflow";
		case ExecutionStatus::InvalidScrIndex:
			return "invalid SCR index";
	}
	return "unknown error";
}
}