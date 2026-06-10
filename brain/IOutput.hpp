#pragma once

#include <string>

class IOutput {
    public:
        virtual ~IOutput() = default;
        virtual void show(const std::string& text) = 0;
};

class TerminalOutput : public IOutput {
    public:
        void show(const std::string& text);
};
