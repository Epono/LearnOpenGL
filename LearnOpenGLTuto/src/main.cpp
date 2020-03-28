#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <iostream>

#include <shader.h>

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

float redValue = 0.0f;
float greenValue = 0.0f;
float blueValue = 0.0f;

float keyRepeatDelay = 0.5f;
float keyRepeatSpacing = 0.05f;
bool keys[350] = { false };

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS ) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_KP_7) == GLFW_PRESS && keys[GLFW_KEY_KP_7] == GLFW_RELEASE) {
		redValue += 0.1f;
	}
	else if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS && keys[GLFW_KEY_KP_4] == GLFW_RELEASE) {
		redValue -= 0.1f;
	}
	else if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS && keys[GLFW_KEY_KP_8] == GLFW_RELEASE) {
		greenValue += 0.1f;
	}
	else if (glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS && keys[GLFW_KEY_KP_5] == GLFW_RELEASE) {
		greenValue -= 0.1f;
	}
	else if (glfwGetKey(window, GLFW_KEY_KP_9) == GLFW_PRESS && keys[GLFW_KEY_KP_9] == GLFW_RELEASE) {
		blueValue += 0.1f;
	}
	else if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS && keys[GLFW_KEY_KP_6] == GLFW_RELEASE) {
		blueValue -= 0.1f;
	}

	// WIREFRAME
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && keys[GLFW_KEY_Z] == GLFW_RELEASE) {
		GLint polygonMode;
		glGetIntegerv(GL_POLYGON_MODE, &polygonMode);
		if (polygonMode == GL_LINE) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else if (polygonMode == GL_FILL) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
	}

	keys[GLFW_KEY_Z] = glfwGetKey(window, GLFW_KEY_Z);

	keys[GLFW_KEY_KP_7] = glfwGetKey(window, GLFW_KEY_KP_7);
	keys[GLFW_KEY_KP_4] = glfwGetKey(window, GLFW_KEY_KP_4);
	keys[GLFW_KEY_KP_8] = glfwGetKey(window, GLFW_KEY_KP_8);
	keys[GLFW_KEY_KP_5] = glfwGetKey(window, GLFW_KEY_KP_5);
	keys[GLFW_KEY_KP_9] = glfwGetKey(window, GLFW_KEY_KP_9);
	keys[GLFW_KEY_KP_6] = glfwGetKey(window, GLFW_KEY_KP_6);
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


	Shader default_shader("shaders/shader.vert", "shaders/shader.frag");
	Shader shader_uniform("shaders/shader_uniform.vert", "shaders/shader_uniform.frag");
	Shader shader_color_attribute("shaders/shader_color_attribute.vert", "shaders/shader_color_attribute.frag");

	// 
	float verticesRectangle[] = {
		 0.9f,  0.9f, 0.0f,  // top right
		 0.9f,  0.5f, 0.0f,  // bottom right
		 0.5f,  0.5f, 0.0f,  // bottom left
		 0.5f,  0.9f, 0.0f   // top left 
	};

	unsigned int indicesRectangle[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	unsigned int VAO[4], VBO[3], EBO[4];
	glGenVertexArrays(4, VAO);
	glGenBuffers(3, VBO);
	glGenBuffers(4, EBO);


	// RECTANGLE
	glBindVertexArray(VAO[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesRectangle), verticesRectangle, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesRectangle), indicesRectangle, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float) /* could be 0 too */, (void*)0);
	glEnableVertexAttribArray(0);

	// TRIANGLE
	float verticesTriangle[] = {
		// positions         // colors
		 0.1f, -0.9f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
		-0.1f, -0.9f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
		 0.0f, -0.7f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
	};

	unsigned int indicesTriangle[] = {
		0, 1, 2,
	};

	glBindVertexArray(VAO[1]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTriangle), verticesTriangle, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesTriangle), indicesTriangle, GL_STATIC_DRAW);

	// Positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float) /* could be 0 too */, (void*)0);
	glEnableVertexAttribArray(0);

	// Colors
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float) /* could be 0 too */, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Unbind (optional if rebind after, but safer)
	//glBindVertexArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	float verticesF[] = {
		 -0.2f, 0.5f, 0.0f,
		 0.0f, 0.5f, 0.0f,
		 0.5f, 0.5f, 0.0f,
		 0.5f, 0.4f, 0.0f,
		 0.0f, 0.4f, 0.0f,
		 0.0f, 0.0f, 0.0f,
		 0.3f, 0.0f, 0.0f,
		 0.3f, -0.1f, 0.0f,
		 0.0f, -0.1f, 0.0f,
		 0.0f, -0.5f, 0.0f,
		 -0.2f, -0.5f, 0.0f
	};

	unsigned int indicesF0[] = {
		0, 1, 10,
		1, 2, 4,
		6, 7, 8
	};

	unsigned int indicesF1[] = {
		1, 9, 10,
		2, 3, 4,
		5, 6, 8
	};

	glBindVertexArray(VAO[2]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesF), verticesF, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesF0), indicesF0, GL_STATIC_DRAW);

	// 0 == layout location.
	// 3 == nb item for the attribute (x, y, z)
	// GL_FLOAT == type
	// GL_FALSE == whether to normalize
	// 0 / size == stride between next attribute
	// 0 == offset
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float) /* could be 0 too */, (void*)0);
	glEnableVertexAttribArray(0);


	glBindVertexArray(VAO[3]);

	//glGenBuffers(1, &VBO3);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesF1), indicesF1, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float) /* could be 0 too */, (void*)0);
	glEnableVertexAttribArray(0);

	// Unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	double previousTime = glfwGetTime();

	// Main loop
	while (!glfwWindowShouldClose(window.get())) {
		double currentTime = glfwGetTime();

		double elapsed = currentTime - previousTime;
		previousTime = currentTime;
		//std::cout << elapsed << " s - FPS: " << 1 / elapsed  << std::endl;

		// input
		processInput(window.get());

		// rendering commands, here
		glClearColor(redValue, greenValue, blueValue, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		 
		// Rectangle
		default_shader.use();
		glBindVertexArray(VAO[0]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glBindVertexArray(0);

		// Bottom Triangle
		shader_color_attribute.use();
		glBindVertexArray(VAO[1]);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		//glBindVertexArray(0);

		// Red F
		default_shader.use();
		glBindVertexArray(VAO[2]);
		glDrawElements(GL_TRIANGLES, 11, GL_UNSIGNED_INT, 0);
		//glBindVertexArray(0);

		// Green F
		shader_uniform.use();
		float greenValue2 = (sin(currentTime) / 2.0f) + 0.5f;
		shader_uniform.setFloat4("ourColor", 0.0f, greenValue2, 0.0f, 1.0f);
		glBindVertexArray(VAO[3]);
		glDrawElements(GL_TRIANGLES, 11, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// check and call events and swap the buffers
		glfwPollEvents();
		glfwSwapBuffers(window.get());
	}

	glDeleteVertexArrays(3, VAO);
	glDeleteBuffers(2, VBO);
	glDeleteBuffers(3, EBO);

	glfwTerminate();

	return 0;
}

void render() {

}

void cleanUp() {

}