#pragma once

#include <string>
#include <vector>
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
        struct TermSize {
            int cols = 80;
            int rows = 24;
        };
        Viewer viewer;
        Scroller scroller;
        Wrapper wrapper;
        int oldWrappedCount;
        std::string allText;
        TermSize termSize;
        TermSize getTermSize();
        void render(RenderStates renderState);
        void typewriteText(const std::string& textToWrite);
    public:
        Render() {
            termSize = getTermSize();
        }
        void appendText(const std::string& textToAppend);
        void scrollUp(int lines);
        void scrollDown(int lines);
};
