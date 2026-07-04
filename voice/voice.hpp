#pragma once

#include "brain/IOutput.hpp"
#include <string>
class VoiceOutput : public IOutput {
    public:
        void show(const std::string& text) override;
};
