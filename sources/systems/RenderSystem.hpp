#pragma once

#include "graphics/RenderBackend.hpp"
#include "graphics/types.hpp"

#include <entt/entity/fwd.hpp>
#include <entt/entt.hpp>
#include <memory>
#include <sys/stat.h>

class RenderSystem {
public:
    RenderSystem(entt::registry& registry);

    void render(const glm::mat4& proj) noexcept;

private:
    void renderCubemap(const glm::mat4& proj) noexcept;
    void renderMeshes(const glm::mat4& proj) noexcept;

private:
    entt::registry& m_registry;

    std::shared_ptr<RenderBackend> m_backend;

    PipelineID m_mainPipe;
    PipelineID m_skyboxPipe;
    PipelineID m_transparentPipe;
    PipelineID m_linesPipe;
    CubemapID m_cubemap;
};