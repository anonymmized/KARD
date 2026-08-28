#pragma once

constexpr int NUM_OF_RESERVED_LINES = 4;

class Scroller {
    private:
        int firstVisible = 0;
        int outputHeight = 0;
        int linesCount = 0;
        int termHeight = 0;
        int maxFirstVisible();
        void clampViewport();
    public:
        void scrollToBottom();
        void scroll(int lines);
        void setLinesCount(int _linesCount) { linesCount = _linesCount; }
        void setTermHeight(int _termHeight) { termHeight = _termHeight; }
        void calculateOutputHeight();
        int getFirstVisible() { return firstVisible; }
        int getOutputHeight() { return outputHeight; }
};
