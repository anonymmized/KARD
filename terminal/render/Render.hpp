#pragma once

#include <string>
#include <vector>

struct State {
    std::vector<std::string> wrappedLines;
    int firstVisible = 0;
    int outputHeight = 0;
};

enum RenderStates {
    Scroll,
    Rewrite
};

const int NUM_OF_RESERVED_LINES = 3;
const int LINES_TO_SCROLL = 1;

class Render {
    private:
        struct TermSize {
            int cols = 80;
            int rows = 24;
        };
        int oldWrappedCount;
        State state;
        std::string allText;
        TermSize termSize;
        TermSize getTermSize();
        std::vector<std::string> wrapText(const std::string& targetText, int width);
        int calculateOutputHeight(int termHeight);
        void clampViewport();
        void scrollToBottom();
        int maxFirstVisible();
        void render(RenderStates renderState);
        int utf8CharLength(unsigned char lead);
        void typewriteText(const std::string& textToWrite);
    public:
        Render() {
            termSize = getTermSize();
        }
        void appendText(const std::string& textToAppend);
        void scrollUp(int lines);
        void scrollDown(int lines);
};
