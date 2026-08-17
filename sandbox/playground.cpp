#include <iostream>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>

struct TermSize {
    int cols = 80;
    int rows = 24;
};

TermSize getTermSize() {
    TermSize termSize;
    winsize ws{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
        termSize.cols = ws.ws_col;
        termSize.rows = ws.ws_row;
    } else {
        perror("ioctl");
    }
    return termSize;
}

std::vector<std::string> splitInput(const std::string& userInput) {
    TermSize termSize = getTermSize();
    std::vector<std::string> splitedStrings;
    std::string line;
    for (char symbol : userInput) {
        if (line.size() >= termSize.cols - 1) {
            line += symbol;
            splitedStrings.push_back(line);
            line.clear();
            continue;
        }
        line += symbol;
        if (symbol == '\n' || symbol == '\r') {
            splitedStrings.push_back(line);
            line.clear();
        }
    }
    return splitedStrings;
}

int main() {
    int firstVisible;
    std::string userInput;
    std::cout << "User: ";
    std::cin >> userInput;
    std::vector<std::string> splittedStrings = splitInput(userInput);
    for (const auto& str : splittedStrings) {
        std::cout << '\t' << str << '\n';
    }
    return 0;
}
