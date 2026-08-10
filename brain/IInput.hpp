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
    public:
        TerminalInput(TerminalSetup& _setup) : setup(_setup) {}
        std::string read();
};
