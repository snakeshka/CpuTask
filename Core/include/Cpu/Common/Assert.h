#pragma once

#include <iostream>
#include <cstdlib>

#ifdef NDEBUG

#define CPU_ASSERT(expression, message) ((void)0)

#else

#define CPU_ASSERT(expression, message)                                    \
	do                                                                     \
	{                                                                      \
		if (!(expression))                                                 \
		{                                                                  \
			std::cerr << "[ASSERT FAILED] " << __FILE__ << ":" << __LINE__ \
					  << "\n  Condition: (" << #expression << ")"          \
					  << "\n  Message: " << (message) << "\n";             \
			std::abort();                                                  \
		}                                                                  \
	} while (0)

#endif