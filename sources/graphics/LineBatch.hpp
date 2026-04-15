#pragma once

#include "utils/types.hpp"

#include <glm/glm.hpp>

#include <vector>

class LineBatch {
public:
    void push(const Line& line) noexcept;
    void draw() const noexcept;

private:
    std::vector<Line> m_lines;
};
