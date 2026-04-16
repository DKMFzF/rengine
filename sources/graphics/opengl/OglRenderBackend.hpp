#pragma once

#include "OglCubemap.hpp"
#include "OglMesh.hpp"
#include "OglPipeline.hpp"
#include "OglTexture.hpp"
#include "graphics/RenderBackend.hpp"
#include "graphics/types.hpp"
#include "utils/types.hpp"
#include <cstdint>
#include <optional>
#include <vector>

class OglRenderBackend : public RenderBackend {
public:
    MeshID createMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) noexcept override;
    TextureID createTexture(const Image& image) noexcept override;
    PipelineID createPipeline(const PipelineParams& params, const RenderState& state) noexcept override;
    CubemapID createCubemap(const std::vector<Image>& images) noexcept override;

    void bindTexture(TextureID texture, int slot = 0) noexcept override;
    void bindPipeline(PipelineID pipeline) noexcept override;

    void drawMesh(MeshID mesh) noexcept override;
    void drawLines(const std::vector<Line>& lines) noexcept override;
    void drawCubemap(CubemapID cubemap) noexcept override;

    void clearColor() noexcept override;
    void clearDepth() noexcept override;
    void setValue(PipelineID pipeline, const std::string& name, Value value) noexcept override;

private:
    void applyState(const RenderState& state) noexcept;

private:
    std::optional<PipelineID> m_bindedPipe;

    std::vector<OglMesh> m_meshes;
    std::vector<OglTexture> m_textures;
    std::vector<OglPipeline> m_pipelines;
    std::vector<OglCubemap> m_cubemaps;
};
