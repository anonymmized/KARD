#include "TerminalBuffer.hpp" 

#include <string>
#include <vector>

void TerminalBuffer::appendMessageToBuffer(const std::string& messageToAppend, MessageRole role) {
    Message newMessage;
    newMessage.messageRole = role;
    newMessage.messageText = messageToAppend;
    messages.push_back(newMessage);
    currentMessageIndex = static_cast<int>(messages.size());
}

Message TerminalBuffer::getPreviousMessage() {
    if (messages.empty()) {
        return {};
    }
    if (currentMessageIndex > 0) {
        currentMessageIndex -= 1;
    }
    return messages[currentMessageIndex];
}

Message TerminalBuffer::getNextMessage() {
    if (messages.empty()) {
        return {};
    }
    int lastMessageIndex = static_cast<int>(messages.size()) - 1;
    int messagesSize = lastMessageIndex + 1;
    if (currentMessageIndex < lastMessageIndex) {
        currentMessageIndex += 1;
    } else if (currentMessageIndex == lastMessageIndex){
        currentMessageIndex = messagesSize;
        return clearMessage;
    } else if (currentMessageIndex == messagesSize) {
        return clearMessage;
    }
    return messages[currentMessageIndex];
}
