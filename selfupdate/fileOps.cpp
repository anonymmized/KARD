#include "selfupdate/fileOps.hpp"

#include <string>
#include <iostream>
#include <filesystem>

void FileOps::copyFile(const std::string& oldFile, const std::string& newFile) {
    std::error_code errorCode;
    std::filesystem::copy_file(oldFile, newFile, std::filesystem::copy_options::overwrite_existing, errorCode);
    if (errorCode) {
        throw std::runtime_error(std::string("copy failed: " + errorCode.message()));
    }
}

void FileOps::renameFile(const std::string& oldName, const std::string& newName) {
    std::error_code errorCode;
    std::filesystem::copy_file(oldName, newName, errorCode);
    if (errorCode) {
        throw std::runtime_error(std::string("rename failed: " + errorCode.message()));
    }
}

void FileOps::removeDirectory(const std::string& targetDirectory) {
    std::error_code errorCode;
    std::filesystem::remove_all(targetDirectory, errorCode);
    if (errorCode) {
        std::cerr << "remove failed: " << errorCode.message() << '\n';
    }
}
