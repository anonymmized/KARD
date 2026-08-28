#pragma once

#include <string>

class Viewer {
    public:
        void clearLine();
        void moveCursor(int line);
        void printLine(const std::string& text);
        void typewriteText(const std::string& text);
};
