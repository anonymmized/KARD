#pragma once

#include <string>

class IOutput {
    public:
        virtual ~IOutput() = default;
        virtual void show(const std::string& text) = 0;
        virtual void startThinking() {}
        virtual void stopThinking() {}
        virtual void showUserText(const std::string& text) {}
};
