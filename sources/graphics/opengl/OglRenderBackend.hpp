#pragma once

#include "graphics/RenderBackend.hpp"

class OglRenderBackend : public RenderBackend {
    void draw(const std::vector<Line>& lines) noexcept override;
    void clear() noexcept override;
};
