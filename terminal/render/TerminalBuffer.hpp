#pragma once

#include <string>
#include <vector>
#include "TerminalMessage.hpp"

class TerminalBuffer {
    private:
        std::vector<Message> messages;
        int currentMessageIndex = -1;
    public:
        void appendMessageToBuffer(const std::string& messageToAppend, MessageRole role);
        Message getPreviousMessage();
        Message getNextMessage();
};
