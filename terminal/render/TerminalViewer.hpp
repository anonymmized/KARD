#pragma once

#include <string>

class Viewer {
    public:
        void clearLine();
        void moveCursor(int line);
        void typewriteText(const std::string& text);
};
