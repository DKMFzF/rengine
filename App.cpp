#include "App.hpp"
#include "Shader.hpp"
#include "Buffer.hpp"
#include "VertexArray.hpp"
#include "Texture.hpp"
#include "Mesh.hpp"
#include "Object.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "tiny_obj_loader.h"

#include <iostream>
#include <stdexcept>
#include <vector>

App::App(int windowWidth, int windowHeight, const std::string& windowTitle) {
	m_window = GlfwWindowPtr(glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(), nullptr, nullptr));
	if (m_window == nullptr) {
		throw std::runtime_error{ "Failed to create GLFW window" };
	}
	m_input.setGlfwWindow(m_window.get());

	glfwSetWindowUserPointer(m_window.get(), this);
	glfwSetCursorPosCallback(m_window.get(), mouseCallback);
	glfwSetInputMode(m_window.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(m_window.get());
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		throw std::runtime_error{ "Failed to init GLEW" };
	}
	
	glViewport(0, 0, windowWidth, windowHeight);
	glEnable(GL_DEPTH_TEST);
}

void App::run() {

	Shader shader{ "main_v.glsl", "main_f.glsl" };

	auto texture = std::make_shared<Texture>("viking_room.png");
	auto mesh = std::make_shared<Mesh>("viking_room.obj");

	Object room{ mesh, texture };
	Object room2{ mesh, texture };

	room.rotation() = { -90.0f, 0.0f, 180.0f };
	room2.rotation() = { -90.0f, 0.0f, 180.0f };

	while (m_running) {
		updateWindow();
		processInput();
		render();
		
		auto proj = glm::perspective(glm::radians(80.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

		auto view = m_camera.getView();

		if (m_input.getKey(GLFW_KEY_LEFT)) room.position() += glm::vec3{ -0.01f, 0.0f, 0.0f };
		if (m_input.getKey(GLFW_KEY_RIGHT)) room.position() += glm::vec3{ 0.01f, 0.0f, 0.0f };
		if (m_input.getKey(GLFW_KEY_UP)) room.position() += glm::vec3{ 0.0f, 0.0f, -0.01f };
		if (m_input.getKey(GLFW_KEY_DOWN)) room.position() += glm::vec3{ 0.0f, 0.0f, 0.01f };

		shader.use();
		shader.setUniform(view, "view");
		shader.setUniform(proj, "proj");

		room.draw(shader);
		room2.draw(shader);

		glfwSwapBuffers(m_window.get());
	}
}

Input& App::getInput() noexcept {
	return m_input;
}

void App::updateWindow() noexcept {
	glfwPollEvents();
}

void App::processInput() noexcept {
	if (m_input.getKey(GLFW_KEY_W))				m_camera.move(Camera::Direction::Front);
	if (m_input.getKey(GLFW_KEY_S))				m_camera.move(Camera::Direction::Back);
	if (m_input.getKey(GLFW_KEY_A))				m_camera.move(Camera::Direction::Left);
	if (m_input.getKey(GLFW_KEY_D))				m_camera.move(Camera::Direction::Right);
	if (m_input.getKey(GLFW_KEY_SPACE))			m_camera.move(Camera::Direction::Up);
	if (m_input.getKey(GLFW_KEY_LEFT_SHIFT))	m_camera.move(Camera::Direction::Down);

	m_camera.update();

	if (m_input.getKey(GLFW_KEY_ESCAPE))
		close();
}

void App::render() noexcept {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void App::close() noexcept {
	m_running = false;
}

void App::mouseCallback(GLFWwindow* window, double xpos, double ypos) noexcept {
	auto* app = static_cast<App*>(glfwGetWindowUserPointer(window));
	if (app->m_firstMouse) {
		app->m_lastX = xpos;
		app->m_lastY = ypos;
		app->m_firstMouse = false;
	}

	float xoffset = xpos - app->m_lastX;
	float yoffset = app->m_lastY - ypos;
	app->m_lastX = xpos;
	app->m_lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	app->m_yaw += xoffset;
	app->m_pitch += yoffset;

	app->m_camera.rotate(app->m_yaw, app->m_pitch);
}
