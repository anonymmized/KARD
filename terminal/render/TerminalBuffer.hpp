#pragma once

#include <string>
#include <vector>
#include "TerminalMessage.hpp"

class TerminalBuffer {
    private:
        std::vector<Message> messages;
        int currentMessageIndex = 0;
        Message clearMessage = {MessageRole::User, ""};
    public:
        void appendMessageToBuffer(const std::string& messageToAppend, MessageRole role);
        Message getPreviousMessage();
        Message getNextMessage();
};
