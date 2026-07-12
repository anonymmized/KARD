#pragma once

#include <string>
#include <atomic>
#include <thread>

class IOutput {
    public:
        virtual ~IOutput() = default;
        virtual void show(const std::string& text) = 0;
        virtual void startThinking() {}
        virtual void stopThinking() {}
};

class TerminalOutput : public IOutput {
    private:
        std::atomic<bool> thinking{false};
        std::thread spinner;
    public:
        void show(const std::string& text);
        void startThinking();
        void stopThinking();
};

class CompositeOutput : public IOutput {
    private:
        IOutput& terminalOutputLink;
        IOutput& voiceOutputLink;
    public:
        CompositeOutput(IOutput& first, IOutput& second) : terminalOutputLink(first), voiceOutputLink(second) {}
        void show(const std::string& text);
        void startThinking();
        void stopThinking();
};
