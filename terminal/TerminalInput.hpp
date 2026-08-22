#pragma once

#include "contracts/IInput.hpp"
#include "terminal/TerminalSetup.hpp"
#include "terminal/render/Render.hpp"
#include <string>

class TerminalInput : public IInput {
    private:
        TerminalSetup& setup;
        Render& render;
        void redraw(const std::string& line);
        std::string readEscapeSequenceForMouse();
        void enableMouseDetection();
        void disableMouseDetection();
    public:
        TerminalInput(TerminalSetup& _setup, Render& _render) : setup(_setup), render(_render) {}
        std::string read();
};
