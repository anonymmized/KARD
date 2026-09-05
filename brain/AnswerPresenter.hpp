#pragma once

#include <string>
#include "contracts/BrainAnswer.hpp"

class Presenter {
    private:
        BrainAnswer brainAnswer;
    public:
        void setBrainAnswer(BrainAnswer _brainAnswer);
        std::string getFinalAnswer();
};
