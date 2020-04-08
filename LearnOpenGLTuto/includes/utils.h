#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>

#include <imgui/imgui.h>
//#include <imgui_bezier.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_interpolation.hpp>

#include <iostream>
#include <vector>
#include <map>
#include <thread>

// https://stackoverflow.com/questions/35793672/use-unique-ptr-with-glfwwindow
// https://gist.github.com/TheOpenDevProject/1662fa2bfd8ef087d94ad4ed27746120
struct glfwDeleter
{
	void operator()(GLFWwindow* wnd)
	{
		glfwDestroyWindow(wnd);
	}
};

void showImguiDemo();
unsigned int createTexture(const std::string& folderPath, const std::string& name, bool gamma = false);

void processInput(GLFWwindow* window, double deltaTime);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void createOpenGLObjects();
void cleanUp();
void createTextures();
void createShaders();
void render(double deltaTime);
void update(double deltaTime);

inline float B0(float t) { return t * t * t; }
inline float B1(float t) { return 3 * t * t * (1 - t); }
inline float B2(float t) { return 3 * t * (1 - t) * (1 - t); }
inline float B3(float t) { return (1 - t) * (1 - t) * (1 - t); }

glm::vec2 getBezier(float t, const glm::vec2& P0, const glm::vec2& P1, const glm::vec2& P2, const glm::vec2& P3);
float getBezierSimplified(float t, const glm::vec2& P1, const glm::vec2& P2);
glm::mat4 lerpProjectionMatrices(const glm::mat4& perspective, const glm::mat4& ortho, float t);

/*
// Too much "callbacky", better to manually check each frame?
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}
*/

/*
//https://gamedev.stackexchange.com/questions/43691/how-can-i-move-an-object-in-an-infinity-or-figure-8-trajectory
float t = glfwGetTime();
float scale = 2 / (3 - cos(2 * t));
float lightPositionOffsetX = 3 * scale * cos(t);
float lightPositionOffsetY = 3 * sin(t);
float lightPositionOffsetZ = 6 * scale * sin(t * 2) / 2;
*/