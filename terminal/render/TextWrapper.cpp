#include "terminal/render/TextWrapper.hpp"

#include <string>
#include <vector>

int Wrapper::utf8CharLength(unsigned char lead) {
    if ((lead & 0x80) == 0x00) return 1;
    if ((lead & 0xE0) == 0xC0) return 2;
    if ((lead & 0xF0) == 0xE0) return 3;
    if ((lead & 0xF8) == 0xF0) return 4;
    return 1;
}

std::vector<std::string> Wrapper::wrapText(const std::string& targetText, int width) {
    std::vector<std::string> wrappedText;
    std::string line;
    int lineWidth = 0;
    for (size_t charIndex = 0; charIndex < targetText.size();) {
        unsigned char lead = static_cast<unsigned char>(targetText[charIndex]);
        int charLen = utf8CharLength(lead);
        if (charIndex + charLen > targetText.size()) {
            charLen = 1;
        }
        std::string symbol = targetText.substr(charIndex, charLen);
        charIndex += charLen;
        if (symbol == "\n" || symbol == "\r") {
            wrappedText.push_back(line);
            line.clear();
            lineWidth = 0;
            continue;
        }
        if (lineWidth >= width - 1) {
            wrappedText.push_back(line);
            line.clear();
            lineWidth = 0;
        }
        line += symbol;
        lineWidth += 1;
    }
    if (!line.empty()) {
        wrappedText.push_back(line);
    }
    return wrappedText;
}
