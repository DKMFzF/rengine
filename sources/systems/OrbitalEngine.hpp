#pragma once

#include "LineBatch.hpp"
#include <entt/entity/fwd.hpp>
#include <entt/entt.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>

class OrbiralEngine {
public:
    OrbiralEngine(entt::registry& registry);
    void update() noexcept;

    struct Step {
        glm::vec3 position;
        glm::vec3 velocity;
    };
    Step orbitalStep(const glm::vec3& center, const glm::vec3& position, const glm::vec3& velocity, float GM, float dt) const noexcept;
    LineBatch calcOrbit(entt::entity object, entt::entity center) const noexcept;

private:
    entt::registry& m_registry;
};