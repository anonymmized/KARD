#pragma once

#include "contracts/IOutput.hpp"
#include "terminal/TerminalSetup.hpp"
#include "terminal/RawMode.hpp"
#include "terminal/render/Render.hpp"
#include "terminal/TerminalSpinner.hpp"

#include <atomic>
#include <thread>
#include <string>

const std::string SPACING = "  ";

class TerminalOutput : public IOutput {
    private:
        TerminalSetup& setup;
        Render& render;
        Spinner spinner;
        std::atomic<bool>& cancelRequesting;
        std::optional<RawMode> rawMode;
        std::string removeSpaces(const std::string& baseline);
    public:
        TerminalOutput(TerminalSetup& _setup, std::atomic<bool> &_cancelRequesting, Render& _render)
            : setup(_setup), cancelRequesting(_cancelRequesting), render(_render) {}
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
