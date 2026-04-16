#pragma once

#include "graphics/Image.hpp"

#include "third-party/stb_image.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

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

inline std::vector<Image> loadCubeImages(const std::filesystem::path& cubePath)
{
    std::vector<std::filesystem::path> faces = {
        cubePath / "right.png",
        cubePath / "left.png",
        cubePath / "top.png",
        cubePath / "bottom.png",
        cubePath / "front.png",
        cubePath / "back.png"
    };
    std::vector<Image> result;
    result.reserve(6);
    for (unsigned int i = 0; i < faces.size(); i++) {
        auto face = faces[i].string();
        result.emplace_back(loadImage(face));
    }
    return result;
}