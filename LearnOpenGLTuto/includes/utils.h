#pragma once

// https://stackoverflow.com/questions/35793672/use-unique-ptr-with-glfwwindow
// https://gist.github.com/TheOpenDevProject/1662fa2bfd8ef087d94ad4ed27746120
struct glfwDeleter
{
	void operator()(GLFWwindow* wnd)
	{
		glfwDestroyWindow(wnd);
	}
};

void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void createOpenGLObjects();
void cleanUp();
void createShaders();
void render(const double currentTime);
void createTexture(GLenum activeTexture, GLuint textureID, const std::string& texturePath, GLint internalFormat, GLenum format);

// Too much "callbacky", better to manually check each frame?
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}