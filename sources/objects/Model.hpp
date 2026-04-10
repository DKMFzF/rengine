#pragma once

#include "Object.hpp"

#include "Mesh.hpp"
#include "Texture.hpp"



class Model : public Object {
public:
    Model(entt::registry& registry, std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture, std::shared_ptr<Texture> specular);

    glm::vec3& position() noexcept;
    glm::vec3& rotation() noexcept;
    glm::vec3& scale() noexcept;

    const glm::vec3& position() const noexcept;
    const glm::vec3& rotation() const noexcept;
    const glm::vec3& scale() const noexcept;

};