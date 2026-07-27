#pragma once

#include <string>
#include <optional>

namespace scheduler::detail {
    bool run(const std::string& command);
    bool writeFile(const std::string& path, const std::string& textToWrite);
    std::optional<std::string> resolveHomeDir();
};
