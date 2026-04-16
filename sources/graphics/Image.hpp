#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#include "Cubemap.hpp"
#include "third-party/stb_image.h"

struct Image {
    std::vector<uint8_t> data;
    uint32_t width;
    uint32_t height;
    uint32_t channels;
};

inline Image loadImage(std::string path)
{
    stbi_set_flip_vertically_on_load(true);

    int width, height, channels;
    stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 4);
    if (!data)
        throw std::runtime_error { "Failed to load texture " + path };

    Image image {
        .data = std::vector<uint8_t>(data, data + width * height * 4),
        .width = static_cast<uint32_t>(width),
        .height = static_cast<uint32_t>(height),
        .channels = static_cast<uint32_t>(channels)
    };

    stbi_image_free(data);
    return image;
}