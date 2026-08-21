#pragma once

struct TermSize {
    int cols = 80;
    int rows = 24;
};

struct State {
    std::vector<std::string> wrappedLines;
    int firstVisible = 0;
    int outputHeight = 0;
};

const int NUM_OF_RESERVED_LINES = 3;
const int LINES_TO_SCROLL = 1;

class Render {
    private:
        State state;
        TermSize termSize;
        TermSize getTermSize();
        std::vector<std::string> wrapText(const std::string& targetText, int width);
        int calculateOutputHeight(int termHeight);
    public:
        Render(const std::string& _outputText) {
            termSize = getTermSize();
            state.wrappedLines = wrapText(_outputText, termSize.cols);
            state.outputHeight = calculateOutputHeight(termSize.rows);
        }
        void render();
        void scrollDown();
        void scrollUp();

};
