#include "terminal/render/Render.hpp"
#include <string>
#include <vector>

Render::Render(TermSize _termSize) {
    termSize = _termSize;
}

void Render::render(RenderStates renderState) {
    for (int i = 0; i < scroller.getOutputHeight(); i++) {
        int lineIndex = scroller.getFirstVisible() + i;
        viewer.moveCursor(i + 1);
        viewer.clearLine();
        if (lineIndex < wrappedLines.size()) {
            if (renderState == RenderStates::Rewrite) {
                if (lineIndex >= oldWrappedCount) {
                    viewer.typewriteText(wrappedLines[lineIndex]);
                } else {
                    viewer.printLine(wrappedLines[lineIndex]);
                }
            } else if (renderState == RenderStates::Scroll) {
                viewer.printLine(wrappedLines[lineIndex]);
            }
        }
    }
}

void Render::appendText(const std::string& textToAppend) {
    allText += textToAppend;
    oldWrappedCount = wrappedLines.size();
    wrappedLines = wrapper.wrapText(allText, termSize.cols);
    scroller.setLinesCount(static_cast<int>(wrappedLines.size()));
    scroller.setTermHeight(termSize.rows);
    scroller.calculateOutputHeight();
    scroller.scrollToBottom();
    render(Rewrite);
}

void Render::scrollUp(int lines) {
    scroller.scroll(lines * -1);
    render(Scroll);
}

void Render::scrollDown(int lines) {
    scroller.scroll(lines);
    render(Scroll);
}
