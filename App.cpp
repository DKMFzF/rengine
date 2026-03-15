#include "App.hpp"
#include "Shader.hpp"
#include "Buffer.hpp"
#include "VertexArray.hpp"

#include <iostream>
#include <stdexcept>

App::App(int windowWidth, int windowHeight, const std::string& windowTitle) {
	m_window = GlfwWindowPtr(glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(), nullptr, nullptr));
	if (m_window == nullptr) {
		throw std::runtime_error{ "Failed to create GLFW window" };
	}

	glfwMakeContextCurrent(m_window.get());
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		throw std::runtime_error{ "Failed to init GLEW" };
	}
	
	glViewport(0, 0, windowWidth, windowHeight);
}

float vertices[] = {
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f
};

void App::run() {
	VertexArray vao;
	vao.bind();

	VertexBuffer buffer{ vertices, sizeof(vertices) };

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	vao.unbind();

	Shader shader{ "main_v.glsl", "main_f.glsl" };

	while (!glfwWindowShouldClose(m_window.get())) {
		updateWindow();
		processInput();
		render();
		
		shader.use();
		vao.bind();
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(m_window.get());
	}
}

void App::updateWindow() noexcept {
	glfwPollEvents();
}

void App::processInput() noexcept {

}

void App::render() noexcept {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}
