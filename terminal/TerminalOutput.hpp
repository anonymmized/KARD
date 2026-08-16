#pragma once

#include "contracts/IOutput.hpp"
#include "terminal/TerminalSetup.hpp"
#include "terminal/RawMode.hpp"

#include <atomic>
#include <thread>
#include <chrono>
#include <string>

constexpr int DELAY = 35;
const std::string SPACING = "  ";

class TerminalOutput : public IOutput {
    private:
        TerminalSetup& setup;
        std::chrono::steady_clock::time_point thinkingStart;
        std::atomic<bool> thinking{false};
        std::atomic<bool> &cancelRequesting;
        std::optional<RawMode> rawMode;
        std::thread spinner;
        std::string doSpacesInText(const std::string& text);
        void typewriteText(const std::string& textToShow);
        std::string removeSpaces(const std::string& baseline);
    public:
        TerminalOutput(std::atomic<bool> &_cancelRequesting, TerminalSetup& _setup)
            : cancelRequesting(_cancelRequesting), setup(_setup) {}
        void show(const std::string& text);
        void startThinking();
        void stopThinking();
        void showUserText(const std::string& text);
};

class CompositeOutput : public IOutput {
    private:
        IOutput& terminalOutputLink;
        IOutput& voiceOutputLink;
    public:
        CompositeOutput(IOutput& terminalOutput, IOutput& voiceOutput) 
            : terminalOutputLink(terminalOutput), voiceOutputLink(voiceOutput) {}
        void show(const std::string& text);
        void startThinking();
        void stopThinking();
        void showUserText(const std::string& text);
};
