#include "terminal/render/TerminalViewer.hpp"

#include <string>
#include <chrono>
#include <thread>
#include <iostream>

void Viewer::clearLine() {
    std::cout << "\033[2K";
}

void Viewer::moveCursor(int line) {
    std::cout << "\033[" << line << ";1H";
}

void Viewer::typewriteText(const std::string& text) {
    for (char symbol : text) {
        std::cout << symbol << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
}
