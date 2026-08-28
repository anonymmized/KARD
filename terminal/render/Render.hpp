#pragma once

#include <string>
#include <vector>
#include "terminal/TerminalSetup.hpp"
#include "terminal/render/ViewportScroller.hpp"
#include "terminal/render/TerminalViewer.hpp"
#include "terminal/render/TextWrapper.hpp"

enum RenderStates {
    Scroll,
    Rewrite
};

const int LINES_TO_SCROLL = 1;

class Render {
    private:
        std::vector<std::string> wrappedLines;
        Viewer viewer;
        Scroller scroller;
        Wrapper wrapper;
        int oldWrappedCount;
        std::string allText;
        TermSize termSize;
        void render(RenderStates renderState);
    public:
        Render(TermSize _termSize);
        void appendText(const std::string& textToAppend);
        void scrollUp(int lines);
        void scrollDown(int lines);
};
