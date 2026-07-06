#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace Assembler {

class SourceFile {
public:
    SourceFile(std::string path, std::string text);

    const std::string& Path() const;
    std::string_view Text() const;
    std::string_view GetLine(size_t line) const;

    static std::optional<SourceFile> readFromDisk(const std::string& path);

private:
    std::string m_Path;
    std::string m_Text;
    std::vector<std::string_view> m_Lines;

    void BuildLineIndex();
};

}
