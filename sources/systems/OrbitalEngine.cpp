#include "OrbitalEngine.hpp"
#include "LineBatch.hpp"
#include "components/Celestial.hpp"
#include "components/OrbitalBody.hpp"
#include "components/Transform.hpp"
#include "systems/PhysicsEngine.hpp"
#include <glm/geometric.hpp>

OrbiralEngine::OrbiralEngine(entt::registry& registry)
    : m_registry { registry }
{
}

void OrbiralEngine::update() noexcept
{
    auto& physics = m_registry.ctx().get<PhysicsEngine>();
    auto dt = 1.0f / 60.0f;

    auto celView = m_registry.view<Celestial, Transform>();
    auto [celestial, celTransform] = m_registry.get<Celestial, Transform>(celView.front());
    for (auto [entity, body, transform] : m_registry.view<OrbitalBody, Transform>().each()) {
        auto step = orbitalStep(celTransform.position, transform.position, body.velocity, celestial.GM, dt);
        transform.position = step.position;
        body.velocity = step.velocity;
        physics.applyTransform(entity);
    }
}

OrbiralEngine::Step OrbiralEngine::orbitalStep(const glm::vec3& center, const glm::vec3& position, const glm::vec3& velocity, float GM, float dt) const noexcept
{
    auto acc = [center, GM](const glm::vec3& pos) {
        auto r = center - pos;
        auto dist = glm::length(r);
        return glm::normalize(r) * (GM / (dist * dist));
    };

    Step step { position, velocity };
    step.velocity += 0.5f * acc(step.position) * dt;
    step.position += step.velocity * dt;
    step.velocity += 0.5f * acc(step.position) * dt;

    return step;
}

LineBatch OrbiralEngine::calcOrbit(entt::entity object, entt::entity center) const noexcept
{
    LineBatch result;
    auto [objBody, objTrans] = m_registry.get<OrbitalBody, Transform>(object);
    auto [celBody, celTrans] = m_registry.get<Celestial, Transform>(center);
    auto dt = 1.0f / 60.0f;

    auto lastStep = Step { objTrans.position, objBody.velocity };
    for (int i = 0; i < 5000; i++) {
        auto step = orbitalStep(celTrans.position, lastStep.position, lastStep.velocity, celBody.GM, dt);
        result.push(Line { lastStep.position, step.position });
        lastStep = step;
    }

    return result;
}