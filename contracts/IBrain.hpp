#pragma once

#include <string>

class IBrain {
    public:
        virtual ~IBrain() = default;
        virtual std::string ask(const std::string& request) = 0;
};
