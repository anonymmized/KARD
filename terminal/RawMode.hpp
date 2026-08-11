#pragma once

#include <termios.h>
#include <unistd.h>

struct RawMode {
    termios original;
    RawMode() {
        tcgetattr(STDIN_FILENO, &original);
        termios raw = original;
        raw.c_lflag &= ~(ICANON | ECHO);
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    }
    ~RawMode() {
        tcsetattr(STDIN_FILENO, TCSANOW, &original); 
    }
};
