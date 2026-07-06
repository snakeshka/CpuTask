#pragma once

#include <string_view>

namespace Emulator
{
enum class ExecutionStatus
{
	Success,
	DivisionByZero,
	IllegalInstruction
};

inline std::string_view ToString(ExecutionStatus status)
{
	switch (status)
	{
		case ExecutionStatus::Success:
			return "Success";
		case ExecutionStatus::DivisionByZero:
			return "division by zero";
		case ExecutionStatus::IllegalInstruction:
			return "illegal instruction";
	}
	return "unknown error";
}

}