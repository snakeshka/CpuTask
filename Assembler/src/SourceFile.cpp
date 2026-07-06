#include "SourceFile.h"

#include <fstream>
#include <sstream>

namespace Assembler {

SourceFile::SourceFile(std::string path, std::string text)
    : m_Path(std::move(path))
    , m_Text(std::move(text))
{
    BuildLineIndex();
}

const std::string& SourceFile::Path() const
{
    return m_Path;
}

std::string_view SourceFile::Text() const
{
    return m_Text;
}

std::string_view SourceFile::GetLine(size_t line) const
{
    if (line == 0 || line > m_Lines.size()) {
        return {};
    }

    return m_Lines[line - 1];
}

std::optional<SourceFile> SourceFile::readFromDisk(const std::string& path)
{
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        return std::nullopt;
    }

    std::ostringstream buffer;
    buffer << input.rdbuf();
    return SourceFile(path, buffer.str());
}

void SourceFile::BuildLineIndex()
{
    m_Lines.clear();

    size_t lineStart = 0;
    for (size_t index = 0; index < m_Text.size(); ++index) {
        if (m_Text[index] == '\n') {
            size_t lineEnd = index;
            if (lineEnd > lineStart && m_Text[lineEnd - 1] == '\r') {
                --lineEnd;
            }
            m_Lines.emplace_back(m_Text.data() + lineStart, lineEnd - lineStart);
            lineStart = index + 1;
        }
    }

    if (lineStart <= m_Text.size()) {
        size_t lineEnd = m_Text.size();
        if (lineEnd > lineStart && m_Text[lineEnd - 1] == '\r') {
            --lineEnd;
        }
        m_Lines.emplace_back(m_Text.data() + lineStart, lineEnd - lineStart);
    }
}

}
