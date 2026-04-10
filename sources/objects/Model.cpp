#include "Model.hpp"
#include "Object.hpp"

#include "components/Renderer.hpp"
#include "components/Transform.hpp"

Model::Model(entt::registry& registry, std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture, std::shared_ptr<Texture> specular)
    : Object(registry)
{
    addComponent(Renderer { mesh, texture, specular });
    addComponent(mesh->getAABB());
}

glm::vec3& Model::position() noexcept { return getComponent<Transform>().position; }
glm::vec3& Model::rotation() noexcept { return getComponent<Transform>().rotation; }
glm::vec3& Model::scale() noexcept { return getComponent<Transform>().scale; }

const glm::vec3& Model::position() const noexcept { return getComponent<Transform>().position; }
const glm::vec3& Model::rotation() const noexcept { return getComponent<Transform>().rotation; }
const glm::vec3& Model::scale() const noexcept { return getComponent<Transform>().scale; }