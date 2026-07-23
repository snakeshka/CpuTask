#pragma once

#include "Diagnostic.h"

#include <vector>

namespace Assembler {
class DiagnosticBag
{
public:
	void Add(Diagnostic diagnostic)
	{
		m_Diagnostics.push_back(std::move(diagnostic));
	}

	bool HasErrors() const
	{
		for (const auto& diagnostic : m_Diagnostics) {
			if (diagnostic.Severity == DiagnosticSeverity::Error) {
				return true;
			}
		}

		return false;
	}

	const std::vector<Diagnostic>& diagnostics() const
	{
		return m_Diagnostics;
	}

private:
	std::vector<Diagnostic> m_Diagnostics;
};
}