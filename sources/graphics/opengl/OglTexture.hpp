#pragma once

#include "../GlHandle.hpp"

#include <cstdint>

struct OglTexture {
    GlTexture texture;
    uint32_t width;
    uint32_t height;
};
