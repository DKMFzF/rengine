#include "OglRenderBackend.hpp"
#include "graphics/Buffer.hpp"
#include "graphics/VertexArray.hpp"

#include <GL/glew.h>

void OglRenderBackend::draw(const std::vector<Line>& lines) noexcept
{
    VertexArray vao;
    vao.bind();
    Buffer<GL_ARRAY_BUFFER, GL_STREAM_DRAW> vertices { lines.data(), lines.size() * sizeof(Line) };

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
        (void*)0);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_LINES, 0, lines.size() * 2);
}

void OglRenderBackend::clear() noexcept
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}