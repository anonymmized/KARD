#include "TerminalSpinner.hpp"
#include <thread>
#include <atomic>
#include <string>
#include <iostream>
#include <unistd.h>

void Spinner::start(std::atomic<bool>& cancelRequesting) {
    cancelRequesting = false;
    thinking = true;
    spinner = std::thread([&] {
            const char *frames[] = {"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"};
            int i = 0;
            while (thinking) {
                printSpinnerElement(frames[i++ % 10]);
                cancelRequesting = detectEscapeToStop();
                std::this_thread::sleep_for(std::chrono::milliseconds(DELAY));
            }
            std::cout << "\r\033[K" << std::flush;
    });
}

void Spinner::printSpinnerElement(const std::string& elementToPrint) {
    std::cout << '\r' << elementToPrint << std::flush;
}

bool Spinner::detectEscapeToStop() {
    char chr;
    if (read(STDIN_FILENO, &chr, 1) == 1 && chr == 27) {
        return true;
    }
    return false;
}

void Spinner::stop() {
    thinking = false;
    if (spinner.joinable()) {
        spinner.join();
    }
}
