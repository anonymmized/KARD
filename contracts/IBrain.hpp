#pragma once

#include <string>
#include "contracts/BrainAnswer.hpp"

class IBrain {
    public:
        virtual ~IBrain() = default;
        virtual BrainAnswer ask(const std::string& request) = 0;
};
