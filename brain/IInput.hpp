#pragma once

#include "core/terminal.hpp"
#include <string>

class IInput {
    public:
        virtual ~IInput() = default;
        virtual std::string read() = 0;
};

class TerminalInput : public IInput {
    private:
        TerminalSetup& setup;
        void redraw(const std::string& line);
    public:
        TerminalInput(TerminalSetup& _setup) : setup(_setup) {}
        std::string read();
};
