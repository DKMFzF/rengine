#pragma once

#include "Image.hpp"
#include "types.hpp"
#include "utils/types.hpp"
#include <cstdint>
#include <vector>

class RenderBackend {
public:
    ~RenderBackend() = default;
    virtual MeshID createMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) noexcept = 0;
    virtual TextureID createTexture(const Image& image) noexcept = 0;

    virtual void bind(TextureID texture, int slot = 0) noexcept = 0;

    virtual void draw(const std::vector<Line>& lines) noexcept = 0;
    virtual void draw(MeshID mesh) noexcept = 0;

    virtual void clear() noexcept = 0;
};