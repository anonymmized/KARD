#pragma once

#include "contracts/IOutput.hpp"
#include "terminal/TerminalSetup.hpp"
#include "terminal/RawMode.hpp"
#include "terminal/render/Render.hpp"

#include <atomic>
#include <thread>
#include <chrono>
#include <string>

constexpr int DELAY = 35;
const std::string SPACING = "  ";

class TerminalOutput : public IOutput {
    private:
        TerminalSetup& setup;
        Render render;
        std::chrono::steady_clock::time_point thinkingStart;
        std::atomic<bool> thinking{false};
        std::atomic<bool> &cancelRequesting;
        std::optional<RawMode> rawMode;
        std::thread spinner;
        std::string doSpacesInText(const std::string& text);
        void typewriteText(const std::string& textToShow);
        std::string removeSpaces(const std::string& baseline);
    public:
        TerminalOutput(TerminalSetup& _setup, std::atomic<bool> &_cancelRequesting)
            : setup(_setup), cancelRequesting(_cancelRequesting) {}
        void show(const std::string& text) override;
        void startThinking() override;
        void stopThinking() override;
        void showUserText(const std::string& text) override;
};

class CompositeOutput : public IOutput {
    private:
        IOutput& terminalOutputLink;
        IOutput& voiceOutputLink;
    public:
        CompositeOutput(IOutput& terminalOutput, IOutput& voiceOutput) 
            : terminalOutputLink(terminalOutput), voiceOutputLink(voiceOutput) {}
        void show(const std::string& text) override;
        void startThinking() override;
        void stopThinking() override;
        void showUserText(const std::string& text) override;
};
