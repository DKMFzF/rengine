#include "RenderSystem.hpp"
#include "BoundingBox.hpp"
#include "Frustum.hpp"
#include "OrbitalEngine.hpp"
#include "components/Camera.hpp"
#include "components/Celestial.hpp"
#include "components/OrbitalBody.hpp"
#include "components/Renderer.hpp"
#include "components/Transform.hpp"
#include "graphics/RenderBackend.hpp"
#include "graphics/opengl/OglRenderBackend.hpp"
#include "utils/utils.hpp"

#include <entt/entity/fwd.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/norm.hpp>

#include <array>
#include <memory>
#include <vector>

RenderSystem::RenderSystem(entt::registry& registry)
    : m_registry { registry }
{
    m_backend.reset(new OglRenderBackend);
    m_registry.ctx().emplace<std::shared_ptr<RenderBackend>>(m_backend);

    m_mainPipe = m_backend->createPipeline({ "resources/shaders/main_v.glsl", "resources/shaders/main_f.glsl" }, RenderState { });
    m_transparentPipe = m_backend->createPipeline({ "resources/shaders/transparent_v.glsl", "resources/shaders/transparent_f.glsl" }, RenderState { });
    m_skyboxPipe = m_backend->createPipeline({ "resources/shaders/cubemap_v.glsl", "resources/shaders/cubemap_f.glsl" }, RenderState { .depth = false });
    m_linesPipe = m_backend->createPipeline({ "resources/shaders/line_v.glsl", "resources/shaders/line_f.glsl" }, RenderState { .depth = false });

    auto cubeImages = loadCubeImages("resources/images/space_skybox");
    m_cubemap = m_backend->createCubemap(cubeImages);
}

std::array<Line, 12> toLines(const BoundingBox& aabb, const Transform& transform) noexcept;
std::array<Line, 12> toLinesAligned(const BoundingBox& aabb, const Transform& transform) noexcept;

void RenderSystem::render(const glm::mat4& proj) noexcept
{
    auto cameraEntity = m_registry.view<Camera, Transform>().front();
    auto [camera, cameraTransform] = m_registry.get<Camera, Transform>(cameraEntity);
    auto view = camera.getView(cameraTransform.position);
    Frustum frustum { proj * view };

    m_backend->clearColor();
    m_backend->clearDepth();

    renderCubemap(proj);
    renderMeshes(proj);

    m_backend->clearDepth();

    m_backend->bindPipeline(m_linesPipe);
    m_backend->setValue(m_linesPipe, "view", view);
    m_backend->setValue(m_linesPipe, "proj", proj);

    std::vector<Line> lines { };
    for (auto [_, bb, transform, renderer] : m_registry.view<BoundingBox, Transform, Renderer, Picked>().each()) {
        auto model = glm::mat4 { 1.0f };
        model = glm::translate(model, transform.position);
        model = glm::scale(model, transform.scale);

        auto linesArray = toLines(bb, transform);
        for (const auto& line : linesArray) {
            lines.push_back(line);
        }

        if (renderer.drawAABB) {
            auto aabb = toGlobalAABB(bb, transform);
            auto alignedLinesArray = toLinesAligned(aabb, transform);
            for (const auto& line : alignedLinesArray) {
                lines.push_back(line);
            }
        }
    }

    m_backend->drawLines(lines);

    auto& orbital = m_registry.ctx().emplace<OrbiralEngine>(m_registry);
    auto celView = m_registry.view<Celestial, Transform>();
    auto [celestial, celTransform] = m_registry.get<Celestial, Transform>(celView.front());
    for (auto [e, transform, body] : m_registry.view<Transform, OrbitalBody>().each()) {
        m_backend->drawLines(body.orbit);
    }
}

void RenderSystem::renderCubemap(const glm::mat4& proj) noexcept
{
    auto cameraEntity = m_registry.view<Camera, Transform>().front();
    auto [camera, cameraTransform] = m_registry.get<Camera, Transform>(cameraEntity);
    auto view = camera.getView(cameraTransform.position);

    auto view_ = glm::mat4(glm::mat3(view));
    m_backend->bindPipeline(m_skyboxPipe);
    m_backend->setValue(m_skyboxPipe, "view", view_);
    m_backend->setValue(m_skyboxPipe, "proj", proj);
    m_backend->drawCubemap(m_cubemap);
}

void RenderSystem::renderMeshes(const glm::mat4& proj) noexcept
{
    auto cameraEntity = m_registry.view<Camera, Transform>().front();
    auto [camera, cameraTransform] = m_registry.get<Camera, Transform>(cameraEntity);
    auto view = camera.getView(cameraTransform.position);

    m_backend->bindPipeline(m_mainPipe);
    m_backend->setValue(m_mainPipe, "view", view);
    m_backend->setValue(m_mainPipe, "proj", proj);

    Frustum frustum { proj * view };

    glm::vec3 lightPos { -15.0f, 15.0f, 15.0f };
    m_backend->setValue(m_mainPipe, "light.position", lightPos);
    m_backend->setValue(m_mainPipe, "light.ambient", glm::vec3 { 0.2f, 0.2f, 0.2f });
    m_backend->setValue(m_mainPipe, "light.diffuse", glm::vec3 { 0.5f, 0.5f, 0.5f });
    m_backend->setValue(m_mainPipe, "light.specular", glm::vec3 { 1.0f, 1.0f, 1.0f });
    m_backend->setValue(m_mainPipe, "viewPos", cameraTransform.position);

    int drawed = 0;
    auto objects = m_registry.view<Transform, Renderer, BoundingBox>(entt::exclude<Transparent>);
    for (auto [entity, transform, renderer, bb] : objects.each()) {
        auto aabb = toGlobalAABB(bb, transform);
        if (!frustum.isVisible(aabb))
            continue;
        drawed++;

        auto model = transform.getMatrix();
        m_backend->setValue(m_mainPipe, "model", model);
        m_backend->setValue(m_mainPipe, "material.ambient", glm::vec3 { 0.8f, 0.8f, 0.8f });
        m_backend->setValue(m_mainPipe, "material.diffuse", 0);
        m_backend->setValue(m_mainPipe, "material.specular", 1);
        m_backend->setValue(m_mainPipe, "material.shininess", 32.0f);

        m_backend->bindTexture(renderer.texture, 0);
        m_backend->bindTexture(renderer.specular, 1);

        auto& meshes = renderer.model->getMeshes();
        for (auto& mesh : meshes) {
            m_backend->drawMesh(mesh.meshID);
        }

        auto front = transform.position + (transform.rotation * glm::vec3(0, 0, -1));
    }

    m_backend->bindPipeline(m_transparentPipe);
    m_backend->setValue(m_transparentPipe, "view", view);
    m_backend->setValue(m_transparentPipe, "proj", proj);

    auto tobjects = m_registry.view<Transform, Renderer, Transparent, BoundingBox>();
    std::vector<std::pair<float, entt::entity>> sorted;
    for (auto entity : tobjects) {
        auto& transform = tobjects.get<Transform>(entity);
        auto dist = glm::distance2(transform.position, cameraTransform.position);
        sorted.push_back({ dist, entity });
    }
    std::sort(sorted.begin(), sorted.end(), [](auto& a, auto& b) {
        return a.first > b.first;
    });

    for (auto [_, entity] : sorted) {
        auto [transform, renderer, bb] = tobjects.get<Transform, Renderer, BoundingBox>(entity);

        auto aabb = toGlobalAABB(bb, transform);
        if (!frustum.isVisible(aabb))
            continue;
        drawed++;

        auto model = transform.getMatrix();
        m_backend->setValue(m_transparentPipe, "model", model);

        m_backend->bindTexture(renderer.texture, 0);
        m_backend->bindTexture(renderer.specular, 1);

        auto& meshes = renderer.model->getMeshes();
        for (auto& mesh : meshes) {
            m_backend->drawMesh(mesh.meshID);
        }

        auto front = transform.position + (transform.rotation * glm::vec3(0, 0, -1));
    }
}

std::array<Line, 12> toLines(const BoundingBox& aabb, const Transform& transform) noexcept
{
    auto model = transform.getMatrix();

    glm::vec3 corners[] = {
        { aabb.min.x, aabb.min.y, aabb.min.z },
        { aabb.max.x, aabb.min.y, aabb.min.z },
        { aabb.min.x, aabb.max.y, aabb.min.z },
        { aabb.max.x, aabb.max.y, aabb.min.z },
        { aabb.min.x, aabb.min.y, aabb.max.z },
        { aabb.max.x, aabb.min.y, aabb.max.z },
        { aabb.min.x, aabb.max.y, aabb.max.z },
        { aabb.max.x, aabb.max.y, aabb.max.z }
    };

    std::array<Line, 12> result;
    result[0] = { corners[0], corners[1] };
    result[1] = { corners[1], corners[3] };
    result[2] = { corners[3], corners[2] };
    result[3] = { corners[2], corners[0] };
    result[4] = { corners[4], corners[5] };
    result[5] = { corners[5], corners[7] };
    result[6] = { corners[7], corners[6] };
    result[7] = { corners[6], corners[4] };
    result[8] = { corners[0], corners[4] };
    result[9] = { corners[1], corners[5] };
    result[10] = { corners[2], corners[6] };
    result[11] = { corners[3], corners[7] };

    for (auto& line : result) {
        line.p1 = glm::vec3 { model * glm::vec4 { line.p1, 1.0f } };
        line.p2 = glm::vec3 { model * glm::vec4 { line.p2, 1.0f } };
    }

    return result;
}

std::array<Line, 12> toLinesAligned(const BoundingBox& aabb, const Transform& transform) noexcept
{
    glm::vec3 corners[] = {
        { aabb.min.x, aabb.min.y, aabb.min.z },
        { aabb.max.x, aabb.min.y, aabb.min.z },
        { aabb.min.x, aabb.max.y, aabb.min.z },
        { aabb.max.x, aabb.max.y, aabb.min.z },
        { aabb.min.x, aabb.min.y, aabb.max.z },
        { aabb.max.x, aabb.min.y, aabb.max.z },
        { aabb.min.x, aabb.max.y, aabb.max.z },
        { aabb.max.x, aabb.max.y, aabb.max.z }
    };

    std::array<Line, 12> result;
    result[0] = { corners[0], corners[1] };
    result[1] = { corners[1], corners[3] };
    result[2] = { corners[3], corners[2] };
    result[3] = { corners[2], corners[0] };
    result[4] = { corners[4], corners[5] };
    result[5] = { corners[5], corners[7] };
    result[6] = { corners[7], corners[6] };
    result[7] = { corners[6], corners[4] };
    result[8] = { corners[0], corners[4] };
    result[9] = { corners[1], corners[5] };
    result[10] = { corners[2], corners[6] };
    result[11] = { corners[3], corners[7] };

    return result;
}