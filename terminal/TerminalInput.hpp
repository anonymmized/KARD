#pragma once

#include "contracts/IInput.hpp"
#include "terminal/TermonalSetup.hpp"
#include <string>

class TerminalInput : public IInput {
    private:
        TerminalSetup& setup;
        void redraw(const std::string& line);
    public:
        TerminalInput(TermonalSetup& _setup) : setup(_setup) {}
        std::string read();
};
