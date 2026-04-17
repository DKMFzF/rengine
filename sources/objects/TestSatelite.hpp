#pragma once

#include "ModelObject.hpp"
#include <entt/entity/fwd.hpp>

class TestSatelite : public ModelObject {
public:
    TestSatelite(entt::registry& regisrty, std::shared_ptr<Model> model, TextureID texture, TextureID specular);
    void update() noexcept;

};