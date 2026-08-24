#include "terminal/TerminalOutput.hpp"
#include "RawMode.hpp"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

void TerminalOutput::show(const std::string& text) {
    if (!setup.isInteractive()) {
        std::cout << text << '\n' << std::flush;
        return;
    }
    render.appendText("KARD: " + text + "\n\n");
}

void TerminalOutput::printSpinner(const std::string& elementToPrint) {
    std::cout << '\r' << setup.getGrey() + elementToPrint + setup.getReset() << std::flush;
}

bool TerminalOutput::detectEscapeToStop() {
    char chr;
    if (read(STDIN_FILENO, &chr, 1) == 1 && chr == 27) {
        return true;
    }
    return false;
}

void TerminalOutput::startSpinner() {
    if (!setup.isInteractive()) {
        return;
    }
    cancelRequesting = false;
    spinnerState.thinking = true;
    rawMode.emplace();
    spinnerState.spinner = std::thread([this] {
    const char *frames[] = {"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"};
        int i = 0;
        while (spinnerState.thinking) {
            printSpinner(frames[i++ % 10]);
            cancelRequesting = detectEscapeToStop();
            std::this_thread::sleep_for(std::chrono::milliseconds(DELAY));
        }
        std::cout << "\r\033[K" << std::flush;
    });
}

void TerminalOutput::stopSpinner() {
    spinnerState.thinking = false;
    if (spinnerState.spinner.joinable()) {
        spinnerState.spinner.join();
    }
    rawMode.reset();
}

void TerminalOutput::showUserText(const std::string& text) {
    std::string newText = removeSpaces(text);
    std::cout << "\033[" << setup.getInputRow() << ";1H\033[2K";
    if (!setup.isInteractive()) {
        std::cout << "> " << newText << '\n' << std::flush;
        return;
    }
    render.appendText(setup.getGrey() + "> " + newText + "\n" + setup.getReset());
}

std::string TerminalOutput::removeSpaces(const std::string& baseline) {
    size_t firstNotSpace = baseline.find_first_not_of(' ');
    if (firstNotSpace == std::string::npos) {
        return "";
    }
    size_t lastNotSpace = baseline.find_last_not_of(' ');
    return baseline.substr(firstNotSpace, lastNotSpace - firstNotSpace + 1);
}

void CompositeOutput::show(const std::string& text) {
    terminalOutputLink.show(text);
    voiceOutputLink.show(text);
}

void CompositeOutput::startSpinner() {
    terminalOutputLink.startSpinner();
    voiceOutputLink.startSpinner();
}

void CompositeOutput::stopSpinner() {
    terminalOutputLink.stopSpinner();
    voiceOutputLink.stopSpinner();
}

void CompositeOutput::showUserText(const std::string& text) {
    terminalOutputLink.showUserText(text);
    voiceOutputLink.showUserText(text);
}
