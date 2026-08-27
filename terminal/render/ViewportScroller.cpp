#include "terminal/render/ViewportScroller.hpp"

void Scroller::calculateOutputHeight() {
    outputHeight = termHeight - NUM_OF_RESERVED_LINES;
}

int Scroller::maxFirstVisible() {
    if (linesCount <= outputHeight) {
        return 0;
    }
    return linesCount - outputHeight;
}

void Scroller::clampViewport() {
    if (firstVisible < 0) {
        firstVisible = 0;
    }
    int maxFirst = maxFirstVisible();
    if (firstVisible > maxFirst) {
        firstVisible = maxFirst;
    }
}

void Scroller::scrollToBottom() {
    firstVisible = maxFirstVisible();
}

void Scroller::up(int lines) {
    firstVisible -= lines;
    clampViewport();
}

void Scroller::down(int lines) {
    firstVisible += lines;
    clampViewport();
}
