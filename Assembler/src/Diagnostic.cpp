#include "Diagnostic.h"

namespace Assembler {
std::string_view ToString(DiagnosticSeverity severity)
{
	switch (severity) {
		case DiagnosticSeverity::Error:
			return "error";
		case DiagnosticSeverity::Warning:
			return "warning";
		case DiagnosticSeverity::Note:
			return "note";
	}

	return "diagnostic";
}
}