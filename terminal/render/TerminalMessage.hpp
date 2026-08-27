#pragma once 

#include <string>

enum class MessageRole {
    User,
    Assistant,
    Tool,
    Status
};

struct Message {
    MessageRole messageRole;
    std::string messageText;
};
