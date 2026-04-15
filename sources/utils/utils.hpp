#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

inline std::string loadFile(const std::filesystem::path& filePath)
{
    auto file = std::ifstream { filePath };
    if (!file.is_open()) {
        throw std::runtime_error { "Failed to open file: " + filePath.string() };
    }
    auto sstr = std::stringstream { };
    sstr << file.rdbuf();
    return sstr.str();
}
