// a glUniform value stays set until modified

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

// Dear ImGui
// https://retifrav.github.io/blog/2019/05/26/sdl-imgui/#dear-imgui
// https://retifrav.github.io/blog/2019/08/04/glfw-dear-imgui/
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui_bezier.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image/stb_image.h>

#include <iostream>
#include <vector>
#include <map>
#include <thread>

#include <utils.h>
#include <shader.h>
#include <camera.h>

// LOGIC
float numberOfUpdatesPerSecond = 60;

// SCREEN
int width = 1600;
int height = 900;

float aspectRatio = (float)width / height;

bool fullscreen = false;
bool vsync = true;

// INPUTS
bool firstMouse = true;

double lastMousePosX = width / 2;
double lastMousePosY = height / 2;

bool lastLeftClick = false;
bool lastRightClick = false;
float scrollSpeed = 2.0f;

float keyRepeatDelay = 0.5f;
float keyRepeatSpacing = 0.05f;
bool keys[350] = { false };

// OpenGL
unsigned int VAO[2], VBO[2], EBO[1];
unsigned int VAOGizmo[2], VBOGizmo[1];
unsigned int textures[3];
std::map<std::string, Shader> shaders;

// Data
glm::vec3 backgroundColor(0.089f, 0.089f, 0.108f);

glm::vec3 planePosition(0.0f, 0.0f, -5.0f);

glm::vec3 lightPosition(0.0f, 5.0f, -5.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

float ambientStrength = 0.1f;
float specularStrength = 0.5f;
float diffuseStrength = 1.0f;
int shininess = 32;

bool	drawPlane = true;
bool	drawCubes = true;
bool	drawLight = true;
float	mixValue = 0.0f;

Camera camera(glm::vec3(0.0f, 1.5f, 10.0f));

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	std::unique_ptr<GLFWwindow, glfwDeleter> window;
	if (!fullscreen) {
		window.reset(glfwCreateWindow(width, height, "LearnOpenGL", nullptr, nullptr));
	}
	else {
		window.reset(glfwCreateWindow(width, height, "LearnOpenGL", glfwGetPrimaryMonitor(), nullptr));
	}

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

	// Viewport inside the window, can spill out ouf window, if smaller than window, takes only a fraction of the window
	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);
	glLineWidth(2.0f);

	// Callbacks
	glfwSetFramebufferSizeCallback(window.get(), framebuffer_size_callback);
	glfwSetScrollCallback(window.get(), scroll_callback);

	// https://discourse.glfw.org/t/newbie-questions-trying-to-understand-glfwswapinterval/1287/2
	glfwSwapInterval(vsync ? 1 : 0);

	// because OpenGL and images have different ideas about y-axis
	stbi_set_flip_vertically_on_load(true);

	createOpenGLObjects();

	createTextures();

	createShaders();

	// setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigWindowsResizeFromEdges = true;

	// setup Dear ImGui style
	ImGui::StyleColorsDark();

	// setup platform/renderer bindings
	std::string glsl_version = "#version 330";
	ImGui_ImplGlfw_InitForOpenGL(window.get(), true);
	ImGui_ImplOpenGL3_Init(glsl_version.c_str());





	double lastFrame = 0.0f;
	double deltaTime = 0.0f;

	// Main loop
	while (!glfwWindowShouldClose(window.get())) {
		double currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		processInput(window.get(), deltaTime);

		// render
		render(deltaTime);

		// check and call events and swap the buffers
		// https://discourse.glfw.org/t/correct-order-for-making-fullscreen-with-poll-events-and-window-refresh-etc/1069
		glfwSwapBuffers(window.get());
		glfwPollEvents();
	}

	cleanUp();

	return 0;
}

void createOpenGLObjects() {
	glGenVertexArrays(2, VAO);
	glGenVertexArrays(2, VAOGizmo);
	glGenBuffers(2, VBO);
	glGenBuffers(1, VBOGizmo);
	glGenBuffers(1, EBO);
	glGenTextures(2, textures);

	// Textured rectange
	float verticesTexturedRectangle[] = {
		// positions          // colors           // texture coords		// normals
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,			0.0f, 0.0f, 1.0f,	// top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,  			0.0f, 0.0f, 1.0f,	// bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,  			0.0f, 0.0f, 1.0f,	// bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,  			0.0f, 0.0f, 1.0f	// top left
	};

	unsigned int indicesTexturedRectangle[] = {
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	glBindVertexArray(VAO[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTexturedRectangle), verticesTexturedRectangle, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesTexturedRectangle), indicesTexturedRectangle, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);


	// https://stackoverflow.com/questions/25349620/use-one-gl-element-array-buffer-to-reference-each-attribute-from-0
	// "When using buffers for your vertex attributes, you need to create a vertex for each unique combination of vertex attributes."
	// https://stackoverflow.com/questions/11148567/rendering-meshes-with-multiple-indices
	// "Therefore, every unique combination of components must have its own separate index."
	float verticesCube[] = {
		// position				// tex coords			// normal
		-0.5f, -0.5f, -0.5f,  	0.0f, 0.0f,  0.0f,  	0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  	1.0f, 0.0f,  0.0f,  	0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  	1.0f, 1.0f,  0.0f,  	0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  	1.0f, 1.0f,  0.0f,  	0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  	0.0f, 1.0f,  0.0f,  	0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  	0.0f, 0.0f,  0.0f,  	0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  	0.0f, 0.0f,  0.0f,  	0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  	1.0f, 0.0f,  0.0f,  	0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  	1.0f, 1.0f,  0.0f,  	0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  	1.0f, 1.0f,  0.0f,  	0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  	0.0f, 1.0f,  0.0f,  	0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  	0.0f, 0.0f,  0.0f,  	0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f,  	1.0f, 0.0f, -1.0f,  	0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  	1.0f, 1.0f, -1.0f,  	0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  	0.0f, 1.0f, -1.0f,  	0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  	0.0f, 1.0f, -1.0f,  	0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  	0.0f, 0.0f, -1.0f,  	0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  	1.0f, 0.0f, -1.0f,  	0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  	1.0f, 0.0f,  1.0f,  	0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  	1.0f, 1.0f,  1.0f,  	0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  	0.0f, 1.0f,  1.0f,  	0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  	0.0f, 1.0f,  1.0f,  	0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  	0.0f, 0.0f,  1.0f,  	0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  	1.0f, 0.0f,  1.0f,  	0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  	0.0f, 1.0f,  0.0f, 		1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  	1.0f, 1.0f,  0.0f, 		1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  	1.0f, 0.0f,  0.0f, 		1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  	1.0f, 0.0f,  0.0f, 		1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  	0.0f, 0.0f,  0.0f, 		1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  	0.0f, 1.0f,  0.0f, 		1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  	0.0f, 1.0f,  0.0f,  	1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  	1.0f, 1.0f,  0.0f,  	1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  	1.0f, 0.0f,  0.0f,  	1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  	1.0f, 0.0f,  0.0f,  	1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  	0.0f, 0.0f,  0.0f,  	1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  	0.0f, 1.0f,  0.0f,  	1.0f,  0.0f
	};

	glBindVertexArray(VAO[1]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCube), verticesCube, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(3);

	// Gizmo
	glBindVertexArray(VAOGizmo[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCube), verticesCube, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(3);


	float verticesLine[] = {
		// position
		 0.0f, 0.0f, 0.0f,
		 0.9f, 0.0f, 0.0f
	};
	glBindVertexArray(VAOGizmo[1]);

	glBindBuffer(GL_ARRAY_BUFFER, VBOGizmo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesLine), verticesLine, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);



	// Unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void createTexture(GLenum activeTexture, GLuint textureID, const std::string& texturePath, GLint internalFormat, GLenum format) {
	glActiveTexture(activeTexture);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// set texture wrapping/filtering options on currently bound texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load and generate texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(0);
}

void createTextures() {
	createTexture(GL_TEXTURE0, textures[0], "assets/container.jpg", GL_RGB, GL_RGB);
	createTexture(GL_TEXTURE1, textures[1], "assets/awesomeface.png", GL_RGBA, GL_RGBA);

	createTexture(GL_TEXTURE0, textures[2], "assets/redstone_lamp.png", GL_RGBA, GL_RGBA);
}

void createShaders() {
	Shader shader_color_uniform("shaders/shader_color_uniform.vert", "shaders/shader_color_uniform.frag");
	Shader shader_color_attribute("shaders/shader_color_attribute.vert", "shaders/shader_color_attribute.frag");

	Shader shader_texture_simple("shaders/shader_texture_simple.vert", "shaders/shader_texture_simple.frag");
	shader_texture_simple.use();
	shader_texture_simple.setInt("texture0", 0);

	Shader shader_texture_phong("shaders/shader_texture_phong.vert", "shaders/shader_texture_phong.frag");
	shader_texture_phong.use();
	shader_texture_phong.setInt("texture0", 0);
	shader_texture_phong.setInt("texture1", 1);

	shaders.insert(std::make_pair("shader_color_uniform", shader_color_uniform));
	shaders.insert(std::make_pair("shader_color_attribute", shader_color_attribute));
	shaders.insert(std::make_pair("shader_texture_phong", shader_texture_phong));
	shaders.insert(std::make_pair("shader_texture_simple", shader_texture_simple));
}





void render(double deltaTime) {
	glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 view = camera.getViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera.FOV), aspectRatio, 0.1f, 100.0f);

	//https://gamedev.stackexchange.com/questions/43691/how-can-i-move-an-object-in-an-infinity-or-figure-8-trajectory
	float t = glfwGetTime();
	float scale = 2 / (3 - cos(2 * t));
	float lightPositionOffsetX = 3 * scale * cos(t);
	float lightPositionOffsetY = 3 * sin(t);
	float lightPositionOffsetZ = 6 * scale * sin(t * 2) / 2;
	glm::vec3 lightPositionOffset(lightPositionOffsetX, lightPositionOffsetY, lightPositionOffsetZ);

	// Render OpenGL
	if (drawPlane) {
		glBindVertexArray(VAO[0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[1]);

		Shader& shader_texture_phong = shaders.find("shader_texture_phong")->second;
		shader_texture_phong.use();
		shader_texture_phong.setFloat("mixValue", mixValue);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(planePosition[0], planePosition[1], planePosition[2]));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 1.0f));

		shader_texture_phong.setMatrixFloat4v("model", 1, model);
		shader_texture_phong.setMatrixFloat4v("view", 1, view);
		shader_texture_phong.setMatrixFloat4v("projection", 1, projection);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		Shader::release();
	}
	if (drawCubes) {
		glBindVertexArray(VAO[1]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[1]);

		Shader& shader_texture_phong = shaders.find("shader_texture_phong")->second;
		shader_texture_phong.use();
		shader_texture_phong.setFloat("mixValue", mixValue);

		shader_texture_phong.setMatrixFloat4v("view", 1, view);
		shader_texture_phong.setMatrixFloat4v("projection", 1, projection);

		shader_texture_phong.setVec3("lightColor", lightColor);
		shader_texture_phong.setVec3("lightPosition", lightPosition + lightPositionOffset);
		shader_texture_phong.setVec3("viewPosition", camera.Position);

		shader_texture_phong.setFloat("ambientStrength", ambientStrength);
		shader_texture_phong.setFloat("specularStrength", specularStrength);
		shader_texture_phong.setFloat("diffuseStrength", diffuseStrength);
		shader_texture_phong.setInt("shininess", shininess);

		glm::vec3 cubePositions[] = {
			glm::vec3(0.0f,  5.0f,  0.0f),
			glm::vec3(2.0f,  10.0f, -15.0f),
			glm::vec3(-1.5f, 3.2f, -2.5f),
			glm::vec3(-3.8f, 3.0f, -12.3f),
			glm::vec3(2.4f, 5.4f, -3.5f),
			glm::vec3(-1.7f,  5.0f, -7.5f),
			glm::vec3(1.3f, 3.0f, -2.5f),
			glm::vec3(1.5f,  7.0f, -2.5f),
			glm::vec3(1.5f,  5.2f, -1.5f),
			glm::vec3(-1.3f,  6.0f, -1.5f)
		};

		for (int i = 0; i < 10; ++i) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			//model = glm::rotate(model, (float)glfwGetTime() * glm::radians(30.0f) + 30 * i, glm::vec3(1.0f, 0.0f, 0.0f));
			//model = glm::rotate(model, (float)glfwGetTime() * glm::radians(20.0f) + 20 * i, glm::vec3(0.0f, 1.0f, 0.0f));
			//model = glm::rotate(model, (float)glfwGetTime() * glm::radians(10.0f) + 10 * i, glm::vec3(0.0f, 0.0f, 1.0f));
			shader_texture_phong.setMatrixFloat4v("model", 1, model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		Shader::release();
	}
	if (drawLight) {
		glBindVertexArray(VAO[1]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[2]);

		Shader& shader_texture_simple = shaders.find("shader_texture_simple")->second;
		shader_texture_simple.use();

		shader_texture_simple.setMatrixFloat4v("view", 1, view);
		shader_texture_simple.setMatrixFloat4v("projection", 1, projection);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, lightPosition + lightPositionOffset);
		model = glm::rotate(model, (float)glfwGetTime() * glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, (float)glfwGetTime() * glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, (float)glfwGetTime() * glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		shader_texture_simple.setMatrixFloat4v("model", 1, model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		Shader::release();
	}
	glBindVertexArray(0);

	// gizmo
	{
		glViewport(0, 0, 100, 100);
		glClear(GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(VAOGizmo[0]);

		Shader& shader_color_uniform = shaders.find("shader_color_uniform")->second;
		shader_color_uniform.use();

		// Fixed postition so that camera position doesn't change render 
		glm::mat4 viewGizmo = glm::mat4(view);
		viewGizmo[3][0] = 0.0f;
		viewGizmo[3][1] = 0.0f;
		viewGizmo[3][2] = -3.0f;
		shader_color_uniform.setMatrixFloat4v("view", 1, viewGizmo);
		glm::mat4 projectionGizmo = glm::perspective(glm::radians(camera.FOV), 1.0f, 0.1f, 100.0f);
		shader_color_uniform.setMatrixFloat4v("projection", 1, projectionGizmo);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		shader_color_uniform.setMatrixFloat4v("model", 1, model);

		shader_color_uniform.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
		shader_color_uniform.setVec3("lightPosition", glm::vec3(3.0f, 2.0, 5.0f));
		shader_color_uniform.setVec3("viewPosition", glm::vec3(0.0f, 0.0f, -3.0f));

		shader_color_uniform.setFloat("ambientStrength", ambientStrength);
		shader_color_uniform.setFloat("specularStrength", specularStrength);
		shader_color_uniform.setFloat("diffuseStrength", diffuseStrength);
		shader_color_uniform.setInt("shininess", shininess);

		shader_color_uniform.setFloat4("ourColor", 0.7f, 0.7f, 0.7f, 1.0f);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		// We want Shadow only for the main cube
		shader_color_uniform.setFloat("ambientStrength", 1.0f);

		// X Cube
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.9f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		shader_color_uniform.setMatrixFloat4v("model", 1, model);
		shader_color_uniform.setFloat4("ourColor", 1.0f, 0.0f, 0.0f, 1.0f);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Y Cube
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		shader_color_uniform.setMatrixFloat4v("model", 1, model);
		shader_color_uniform.setFloat4("ourColor", 0.0f, 1.0f, 0.0f, 1.0f);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Z Cube
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.9f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		shader_color_uniform.setMatrixFloat4v("model", 1, model);
		shader_color_uniform.setFloat4("ourColor", 0.0f, 0.0f, 1.0f, 1.0f);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(VAOGizmo[1]);

		// x
		model = glm::mat4(1.0f);
		shader_color_uniform.setMatrixFloat4v("model", 1, model);
		shader_color_uniform.setFloat4("ourColor", 1.0f, 0.0f, 0.0f, 1.0f);
		glDrawArrays(GL_LINES, 0, 6);

		// y
		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		shader_color_uniform.setMatrixFloat4v("model", 1, model);
		shader_color_uniform.setFloat4("ourColor", 0.0f, 1.0f, 0.0f, 1.0f);
		glDrawArrays(GL_LINES, 0, 6);

		// z
		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		shader_color_uniform.setMatrixFloat4v("model", 1, model);
		shader_color_uniform.setFloat4("ourColor", 0.0f, 0.0f, 1.0f, 1.0f);
		glDrawArrays(GL_LINES, 0, 6);

		Shader::release();
		glViewport(0, 0, width, height);
		glDepthFunc(GL_LESS);
	}


	// Render Dear Imgui
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_FirstUseEver);
	ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_None);
	if (ImGui::CollapsingHeader("Colors")) {
		ImGui::ColorEdit3("Background color", &backgroundColor[0], ImGuiColorEditFlags_Float);
		ImGui::ColorEdit3("Light color", &lightColor[0], ImGuiColorEditFlags_Float);
		ImGui::DragFloat3("Light position", &lightPosition[0], 0.1f, -10.0f, 10.0f);
		ImGui::SliderFloat("Ambient Strength", &ambientStrength, 0.0f, 1.0f, "%.2f");
		ImGui::SliderFloat("Specular Strength", &specularStrength, 0.0f, 1.0f, "%.2f");
		ImGui::SliderFloat("Diffuse Strength", &diffuseStrength, 0.0f, 1.0f, "%.2f");
		ImGui::SliderInt("Shininess", &shininess, 2, 256);
	}

	if (ImGui::CollapsingHeader("Draws", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Checkbox("Draw Textured Rectangle?", &drawPlane);
		ImGui::Checkbox("Draw Textured Cube?", &drawCubes);
		ImGui::Checkbox("Draw Light Cube?", &drawLight);

		ImGui::SliderFloat("Mix Value", &mixValue, 0.0f, 1.0f);

		ImGui::DragFloat3("Plane position", &planePosition[0], 0.1f, -10.0f, 10.0f);

		ImGui::Separator();
		ImGui::SliderFloat("FOV", &camera.FOV, 10.0f, 180.0f);
		ImGui::SliderFloat("Camera Speed", &camera.MovementSpeed, 1.0f, 20.f);
		ImGui::Text("Camera: (%.2f, %.2f, %.2f)", camera.Position.x, camera.Position.y, camera.Position.z);

		ImGui::Separator();

		static std::vector<float> values(100, 0);
		values.push_back(deltaTime * 1000);
		if (values.size() > 100) {
			values.erase(values.begin());
		}

		char buff[16];
		snprintf(buff, sizeof(buff), "%2.2f ms", deltaTime * 1000);
		ImGui::PlotLines("Frame times", values.data(), 100, 0.0f, buff, 0.0f, 20.0f, ImVec2(0, 80));
		//https://stackoverflow.com/questions/28530798/how-to-make-a-basic-fps-counter
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void cleanUp() {
	glDeleteVertexArrays(2, VAO);
	glDeleteVertexArrays(2, VAOGizmo);
	glDeleteBuffers(2, VBO);
	glDeleteBuffers(1, EBO);
	glDeleteTextures(2, textures);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, double deltaTime) {
	// MOUSE
	double currentMousePosX;
	double currentMousePosY;
	glfwGetCursorPos(window, &currentMousePosX, &currentMousePosY);

	bool isLeftMousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1);
	bool isRightMousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2);

	if (firstMouse) {
		lastMousePosX = currentMousePosX;
		lastMousePosY = currentMousePosY;
		firstMouse = false;
	}

	double deltaX = currentMousePosX - lastMousePosX;
	double deltaY = lastMousePosY - currentMousePosY; // reversed since y-coordinates go from bottom to top

	if (isRightMousePressed && !lastRightClick) {
		// Just pressed right click
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		firstMouse = true;
	}
	else if (!isRightMousePressed && lastRightClick) {
		// Just released right click
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	// Additional checks to ignore clicks on ImGui windows
	if (isRightMousePressed && lastRightClick && !ImGui::IsAnyWindowHovered() && !ImGui::IsAnyItemHovered()) {
		camera.processMouseMovement(deltaTime, deltaX, deltaY);
	}

	lastMousePosX = currentMousePosX;
	lastMousePosY = currentMousePosY;

	lastLeftClick = isLeftMousePressed;
	lastRightClick = isRightMousePressed;

	// KEYBOARD
	// EXIT
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	// CAMERA
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processKeyboard(CameraMovement::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processKeyboard(CameraMovement::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processKeyboard(CameraMovement::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processKeyboard(CameraMovement::RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.processKeyboard(CameraMovement::UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		camera.processKeyboard(CameraMovement::DOWN, deltaTime);


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

	// FULLSCREEN / WINDOWED FULLSCREEN
	// https://www.glfw.org/docs/latest/window.html#window_windowed_full_screen
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && keys[GLFW_KEY_F] == GLFW_RELEASE) {
		// https://www.glfw.org/docs/3.3/monitor_guide.html
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		// TODO: glfwget ?
		if (fullscreen) {
			glfwSetWindowMonitor(window, nullptr, (mode->width - width) / 2, (mode->height - height) / 2, width, height, mode->refreshRate);
		}
		else {
			glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), NULL, NULL, mode->width, mode->height, mode->refreshRate);
		}
		fullscreen = !fullscreen;
	}

	// Update states
	keys[GLFW_KEY_Z] = glfwGetKey(window, GLFW_KEY_Z);
	keys[GLFW_KEY_F] = glfwGetKey(window, GLFW_KEY_F);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (!ImGui::IsAnyWindowHovered() && !ImGui::IsAnyItemHovered()) {
		camera.processMouseScroll(yoffset * scrollSpeed);
	}
}