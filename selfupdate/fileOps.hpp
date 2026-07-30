#pragma once

#include <string>

class FileOps {
    public:
        void copyFile(const std::string& oldFile, const std::string& newFile);
        void renameFile(const std::string& oldName, const std::string& newName);
        void removeDirectory(const std::string& targetDirectory);
};
