#pragma once

#include "utils/types.hpp"
#include <glm/glm.hpp>
#include <vector>

struct OrbitalBody {
    glm::vec3 velocity { };
    std::vector<Line> orbit;
};
