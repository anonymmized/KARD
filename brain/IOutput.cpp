#include "IOutput.hpp"

#include <iostream>
#include <string>
#include <chrono>
#include <thread>

void TerminalOutput::show(const std::string& text) {
    std::cout << "Assistant: " << text << '\n' << "user: "; // issue №22
}

void TerminalOutput::stopThinking() {
    thinking = false;
    if (spinner.joinable()) spinner.join();
}

void TerminalOutput::startThinking() {
    thinking = true;
    spinner = std::thread([this]{
        const char* frames[] = {"⠋","⠙","⠹","⠸","⠼","⠴","⠦","⠧","⠇","⠏"};
        int i = 0;
        while (thinking) {
            std::cout << '\r' << frames[i++ % 10] << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(DELAY));
        }
        std::cout << "\r\033[K" << std::flush;
    });
}

void CompositeOutput::show(const std::string& text) {
    terminalOutputLink.show(text);
    voiceOutputLink.show(text);
}

void CompositeOutput::startThinking() {
    terminalOutputLink.startThinking();
    voiceOutputLink.startThinking();
}

void CompositeOutput::stopThinking() {
    terminalOutputLink.stopThinking();
    voiceOutputLink.stopThinking();
}
