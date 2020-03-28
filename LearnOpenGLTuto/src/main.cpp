#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <iostream>

// https://stackoverflow.com/questions/35793672/use-unique-ptr-with-glfwwindow
// https://gist.github.com/TheOpenDevProject/1662fa2bfd8ef087d94ad4ed27746120
struct glfwDeleter
{
	void operator()(GLFWwindow* wnd)
	{
		glfwDestroyWindow(wnd);
	}
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

float redValue		= 0.0f;
float greenValue	= 0.0f;
float blueValue		= 0.0f;

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	else if (glfwGetKey(window, GLFW_KEY_KP_7) == GLFW_PRESS) {
		redValue += 0.1f;
	}
	else if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS) {
		redValue -= 0.1f;
	}
	else if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS) {
		greenValue += 0.1f;
	}
	else if (glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS) {
		greenValue -= 0.1f;
	}
	else if (glfwGetKey(window, GLFW_KEY_KP_9) == GLFW_PRESS) {
		blueValue += 0.1f;
	}
	else if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS) {
		blueValue -= 0.1f;
	}
}

int main() {
	std::cout << "lol" << std::endl;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	std::unique_ptr<GLFWwindow, glfwDeleter> window;
	window.reset(glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr));

	//GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window.get());

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);

	// Callbacks
	glfwSetFramebufferSizeCallback(window.get(), framebuffer_size_callback);

	// Too much "callbacky", better to manually check each frame?
	//glfwSetKeyCallback(window.get(), key_callback);

	// Main loop
	while (!glfwWindowShouldClose(window.get())) {

		// input
		processInput(window.get());

		// rendering commands, here
		glClearColor(redValue, greenValue, blueValue, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// check and call events and swap the buffers
		glfwPollEvents();
		glfwSwapBuffers(window.get());
	}

	glfwTerminate();

	return 0;
}