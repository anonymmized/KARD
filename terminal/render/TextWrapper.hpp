#pragma once 

#include <string>
#include <vector>

class Wrapper {
    private:
        int utf8CharLength(unsigned char lead);
    public:
        std::vector<std::string> wrapText(const std::string& targetText, int width);
};
