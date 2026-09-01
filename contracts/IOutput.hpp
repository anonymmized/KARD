#pragma once

#include <string>
#include "contracts/BrainAnswer.hpp"

class IOutput {
    public:
        virtual ~IOutput() = default;
        virtual void show(const std::string& text) = 0;
        virtual void showAnswer(const BrainAnswer& brainAnswer) { show(brainAnswer.plainAnswer); }
        virtual void startThinking() {}
        virtual void stopThinking() {}
        virtual void showUserText(const std::string& text) {}
};
