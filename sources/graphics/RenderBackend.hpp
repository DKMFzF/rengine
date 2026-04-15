#pragma once

#include "utils/types.hpp"
#include <vector>

class RenderBackend {
public:
    ~RenderBackend() = default;
    virtual void draw(const std::vector<Line>& lines) noexcept = 0;
    virtual void clear() noexcept = 0;
};