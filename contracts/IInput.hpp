#pragma once

#include <string>

class IInput {
    public:
        virtual ~IInput() = default;
        virtual std::string read() = 0;
};
