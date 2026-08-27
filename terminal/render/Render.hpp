#pragma once

#include <string>
#include <vector>
#include "terminal/render/ViewportScroller.hpp"
#include "terminal/render/TextWrapper.hpp"

struct State {
    std::vector<std::string> wrappedLines;
    int firstVisible = 0;
    int outputHeight = 0;
};

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
        Scroller scroller;
        Wrapper wrapper;
        int oldWrappedCount;
        State state;
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
