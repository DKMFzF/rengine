#pragma once

#include "Model.hpp"
#include "graphics/types.hpp"

#include <memory>

struct Renderer {
    std::shared_ptr<Model> model;
    TextureID texture;
    TextureID specular;
    bool drawAABB = false;
};

struct Transparent { };