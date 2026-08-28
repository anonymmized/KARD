#include "terminal/render/TerminalMessage.hpp"
#include "terminal/render/TerminalBuffer.hpp"
#include "terminal/TerminalInput.hpp"
#include "terminal/RawMode.hpp"

#include <iostream>
#include <chrono>
#include <thread>

std::string TerminalInput::read() {
    if (!setup.isInteractive()) {
        std::string line;
        if (!std::getline(std::cin, line)) {
            return "/exit";
        }
        return line;
    }
    enableMouseDetection();
    RawMode rawMode;
    std::string line;
    redraw(line);

    while (true) {
        char symbol;
        if (::read(STDIN_FILENO, &symbol, 1) != 1) {
            continue;
        }
        if (symbol == '\n' || symbol == '\r') {
            break;
        }
        if (symbol == 27) {
            std::string sequence = readEscapeSequenceForMouse();
            if (sequence.rfind("[<64;", 0) == 0) {
                render.scrollUp(1);
                redraw(line);
                continue;
            }
            if (sequence.rfind("[<65;", 0) == 0) {
                render.scrollDown(1);
                redraw(line);
                continue;
            }
            if (sequence == "[A") {
                Message previousMessage = buffer.getPreviousMessage();
                line = previousMessage.messageText;
                redraw(line);
                continue;
            }
            if (sequence == "[B") {
                Message nextMessage = buffer.getNextMessage();
                line = nextMessage.messageText;
                redraw(line);
                continue;
            }
        }
        if (symbol == 127 || symbol == 8) {
            if (!line.empty()) {
                while (line.size() > 1 && ((unsigned char)line.back() & 0xC0) == 0x80) {
                    line.pop_back();
                }
                line.pop_back();
            }
        } else if ((unsigned char)symbol >= 32) {
            line += symbol;
        }
        redraw(line);
    }
    disableMouseDetection();
    buffer.appendMessageToBuffer(line, MessageRole::User);
    return line;
}

void TerminalInput::enableMouseDetection() {
    std::cout << "\033[?1000h";
    std::cout << "\033[?1006h";
}

void TerminalInput::disableMouseDetection() {
    std::cout << "\033[?1000l";
    std::cout << "\033[?1006l";
    std::cout << std::flush;
}

std::string TerminalInput::readEscapeSequenceForMouse() {
    std::string sequence;

    for (int i = 0; i < 100; ++i) {
        char symbol;
        if (::read(STDIN_FILENO, &symbol, 1) != 1) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        sequence += symbol;
        if (symbol == 'M' || symbol == 'm' || symbol == '~' || symbol >= 'A' && symbol <= 'Z') {
            break;
        }
        if (sequence.size() >= 64) {
            break;
        }
    }
    return sequence;
}

void TerminalInput::redraw(const std::string& line) {
    int row = setup.getInputRow();
    int cols = setup.getCols();
    int visible = cols - 4;
    if (visible < 1) {
        visible = 1;
    }
    size_t start = 0;
    if (line.size() > (size_t)visible) {
        start = line.size() - (size_t)visible;
        while (start < line.size() && ((unsigned char)line[start] & 0xC0) == 0x80) {
            start += 1;
        }
    }
    std::cout << "\033[" << row << ";1H\033[2K" << "> " << line.substr(start) << std::flush;
}
