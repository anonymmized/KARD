#include "terminal/TerminalOutput.hpp"
#include "RawMode.hpp"

#include <iostream>
#include <string>
#include <thread>


void TerminalOutput::showAnswer(const BrainAnswer& brainAnswer) {
    if (!brainAnswer.textAnswer.empty()) {
        show(brainAnswer.textAnswer);
    }
    for (const Metric& metric : brainAnswer.metrics) {
        std::string line = metric.name + ": " + metric.value;
        if (!metric.unit.empty()) {
            line += " " + metric.unit;
        }
        show (line);
    }
}

void TerminalOutput::show(const std::string& text) {
    if (!setup.isInteractive()) {
        std::cout << text << '\n' << std::flush;
        return;
    }
    render.appendText(text + "\n\n");
}

void TerminalOutput::startThinking() {
    if (!setup.isInteractive()) {
        return;
    }
    rawMode.emplace();
    std::cout << setup.getGrey();
    spinner.start(cancelRequesting);
    std::cout << setup.getReset();
}

void TerminalOutput::stopThinking() {
    spinner.stop();
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

void CompositeOutput::startThinking() {
    terminalOutputLink.startThinking();
    voiceOutputLink.startThinking();
}

void CompositeOutput::stopThinking() {
    terminalOutputLink.stopThinking();
    voiceOutputLink.stopThinking();
}

void CompositeOutput::showUserText(const std::string& text) {
    terminalOutputLink.showUserText(text);
    voiceOutputLink.showUserText(text);
}

void CompositeOutput::showAnswer(const BrainAnswer& brainAnswer) {
    terminalOutputLink.showAnswer(brainAnswer);
}
