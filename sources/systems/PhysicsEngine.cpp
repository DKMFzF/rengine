#include "PhysicsEngine.hpp"
#include "components/Body.hpp"
#include "components/Transform.hpp"

#include <Jolt/Core/Factory.h>
#include <Jolt/Core/Memory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Geometry/Plane.h>
#include <Jolt/Math/Real.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/PlaneShape.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>

PhysicsEngine::PhysicsEngine(entt::registry& registry, JPH::TempAllocatorImpl& tempAllocator, JPH::JobSystemThreadPool& jobSystem)
    : m_registry { registry }
    , m_tempAllocator { tempAllocator }
    , m_jobSystem { jobSystem }
{
    m_world.Init(1024,
        0,
        1024,
        1024,
        m_broad_phase_layer_interface,
        m_object_vs_bp_filter,
        m_object_layer_pair_filter);

    m_registry.storage<Body>();
    m_registry.on_destroy<Body>().connect<&PhysicsEngine::destroyBody>(this);
}

void PhysicsEngine::update() noexcept
{
    m_world.Update(1.0f / 180.0f, 1, &m_tempAllocator, &m_jobSystem);
    auto view = m_registry.view<Body, Transform>();
    for (auto [entity, body, transform] : view.each()) {
        JPH::RVec3 position;
        JPH::Quat rotation;
        auto& ibody = m_world.GetBodyInterface();
        ibody.GetPositionAndRotation(body.bodyID, position, rotation);
        transform.position = { position.GetX(), position.GetY(), position.GetZ() };
    }
}

Body PhysicsEngine::createCube() noexcept
{
    JPH::BoxShapeSettings box_shape(JPH::Vec3(0.25f, 0.25f, 0.25f));
    JPH::BodyCreationSettings box_settings(
        box_shape.Create().Get(),
        JPH::Vec3(0, 4, 0),
        JPH::Quat::sIdentity(),
        JPH::EMotionType::Dynamic,
        1);

    JPH::Body* box = m_world.GetBodyInterface().CreateBody(box_settings);
    m_world.GetBodyInterface().AddBody(box->GetID(), JPH::EActivation::Activate);
    return { box->GetID() };
}

Body PhysicsEngine::createPlane() noexcept
{
    JPH::BoxShapeSettings floor_shape(JPH::Vec3(2.5f, 0.25f, 2.5f));
    JPH::BodyCreationSettings floor_settings(
        floor_shape.Create().Get(),
        JPH::Vec3(0, -0.25, 0),
        JPH::Quat::sIdentity(),
        JPH::EMotionType::Static,
        0);

    JPH::Body* floor = m_world.GetBodyInterface().CreateBody(floor_settings);
    m_world.GetBodyInterface().AddBody(floor->GetID(), JPH::EActivation::DontActivate);
    return { floor->GetID() };
}

void PhysicsEngine::destroyBody(entt::registry& registry, entt::entity entity)
{
    auto& ibody = m_world.GetBodyInterface();
    auto& body = m_registry.get<Body>(entity);
    ibody.RemoveBody(body.bodyID);
    ibody.DestroyBody(body.bodyID);
}