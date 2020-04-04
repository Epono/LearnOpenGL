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
#include <glm/gtx/matrix_interpolation.hpp>

#include <stb_image/stb_image.h>

#include <iostream>
#include <vector>
#include <map>
#include <thread>

#include <utils.h>
#include <shader.h>
#include <camera.h>
#include <directional_light.h>
#include <point_light.h>
#include <spot_light.h>

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
bool lastMiddleClick = false;
float scrollSpeed = 2.0f;

float keyRepeatDelay = 0.5f;
float keyRepeatSpacing = 0.05f;
bool keys[350] = { false };

// OpenGL
unsigned int VAO[2], VBO[2], EBO[1];
unsigned int VAOGizmo[2], VBOGizmo[1];
std::map<std::string, Shader> shaders;

unsigned int texture_container;
unsigned int texture_awesomeface;
unsigned int texture_redstoneLamp;
unsigned int texture_container2;
unsigned int texture_container2Specular;
unsigned int texture_matrix;

std::vector<unsigned int> textures;

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
bool	drawTexturedCubes = true;
bool	drawMaterialCubes = true;
bool	drawLights = true;
bool	drawGizmo = false;
float	mixValue = 0.0f;

Camera camera(glm::vec3(0.0f, 2.5f, 10.0f));

DirectionalLight directionalLight;
std::vector<PointLight> pointLights{ PointLight(glm::vec3(-2.5f, 5.0f, -5.0f)), PointLight(glm::vec3(2.5f, 5.0f, -5.0f)) };
std::vector<SpotLight> spotLights{ SpotLight(glm::vec3(0.0f, 2.0f, -5.0f), glm::vec3(0.0f, -1.0f, 0.0f)) };

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);		// Antialiasing (MSAA)

	std::unique_ptr<GLFWwindow, glfwDeleter> window;
	if (!fullscreen) {
		window.reset(glfwCreateWindow(width, height, "LearnOpenGL", nullptr, nullptr));
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwSetWindowPos(window.get(), (mode->width - width) / 2, (mode->height - height) / 2);
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

	// Culling
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	// Antialiasing (MSAA)
	glEnable(GL_MULTISAMPLE);

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



	// https://gafferongames.com/post/fix_your_timestep/
	int logicStepsPerSecond = 60;
	double dt = (float)1 / logicStepsPerSecond;
	double accumulator = 0.0f;

	double lastFrame = 0.0f;						// current_time
	double deltaTime = 0.0f;						// frame_time

	// Main loop
	while (!glfwWindowShouldClose(window.get())) {
		double currentFrame = glfwGetTime();		// new_time
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		accumulator += deltaTime;

		while (accumulator >= dt) {
			// input
			// & update ideally
			processInput(window.get(), dt);
			update(dt);
			accumulator -= dt;
		}

		// TODO
		GLenum error = glGetError();
		if (error != 0) {
			std::cout << "ERROR: " << error << std::endl;
		}


		// render
		render(dt);

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

	// PLANE
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

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// CUBE
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

	glBindVertexArray(VAOGizmo[1]);

	glBindBuffer(GL_ARRAY_BUFFER, VBOGizmo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesLine), verticesLine, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);



	// Unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void createTextures() {
	texture_container = createTexture("assets/container.jpg");
	textures.push_back(texture_container);

	texture_awesomeface = createTexture("assets/awesomeface.png");
	textures.push_back(texture_awesomeface);

	texture_redstoneLamp = createTexture("assets/redstone_lamp.png");
	textures.push_back(texture_redstoneLamp);

	texture_container2 = createTexture("assets/container2.png");
	textures.push_back(texture_container2);

	texture_container2Specular = createTexture("assets/container2_specular.png");
	textures.push_back(texture_container2Specular);

	texture_matrix = createTexture("assets/matrix.jpg");
	textures.push_back(texture_matrix);
}


void createShaders() {
	Shader shader_color_uniform("shaders/shader_color_uniform.vert", "shaders/shader_color_uniform.frag");
	Shader shader_color_attribute("shaders/shader_color_attribute.vert", "shaders/shader_color_attribute.frag");
	//Shader shader_color_material("shaders/shader_color_material.vert", "shaders/shader_color_material.frag");

	Shader shader_texture_simple("shaders/shader_texture_simple.vert", "shaders/shader_texture_simple.frag");
	shader_texture_simple.use();
	shader_texture_simple.setInt("texture0", 0);

	//Shader shader_texture_phong("shaders/shader_texture_phong.vert", "shaders/shader_texture_phong.frag");
	//shader_texture_phong.use();
	//shader_texture_phong.setInt("texture0", 0);
	//shader_texture_phong.setInt("texture1", 1);

	Shader shader_texture_phong_materials("shaders/shader_texture_phong_materials.vert", "shaders/shader_texture_phong_materials.frag");
	shader_texture_phong_materials.use();
	shader_texture_phong_materials.setInt("material.diffuse", 0);
	shader_texture_phong_materials.setInt("material.specular", 1);
	shader_texture_phong_materials.setInt("material.emission", 2);

	Shader shader_color_phong_materials("shaders/shader_color_phong_materials.vert", "shaders/shader_color_phong_materials.frag");
	shader_color_phong_materials.use();
	shader_color_phong_materials.setInt("material.emission", 2);


	shaders.insert(std::make_pair("shader_color_uniform", shader_color_uniform));
	shaders.insert(std::make_pair("shader_color_attribute", shader_color_attribute));
	//shaders.insert(std::make_pair("shader_color_material", shader_color_material));
	//shaders.insert(std::make_pair("shader_texture_phong", shader_texture_phong));
	shaders.insert(std::make_pair("shader_texture_simple", shader_texture_simple));
	shaders.insert(std::make_pair("shader_texture_phong_materials", shader_texture_phong_materials));
	shaders.insert(std::make_pair("shader_color_phong_materials", shader_color_phong_materials));
}

void update(double deltaTime) {
	//spotLights[0]
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, spotLights[0].Position);
	model = glm::rotate(model, (float)glm::radians(glfwGetTime()), glm::vec3(0.0f, 0.0f, 1.0f));
}

void render(double deltaTime) {
	glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glm::mat4 view = camera.getViewMatrix();

	//////////////////////// 
	// Fun with projections

	glm::mat4 projectionPerspective = glm::perspective(glm::radians(camera.FOV), aspectRatio, 0.1f, 100.0f);
	glm::mat4 projectionOrtho = glm::ortho(
		-aspectRatio * camera.OrthographicFactor, aspectRatio * camera.OrthographicFactor,
		-camera.OrthographicFactor, camera.OrthographicFactor,
		0.1f, 100.0f);

	//////////////////////////////////////////////////////////////
	// mixValue == 1 => full ortho
	//			== 0 => full perspective
	// Projection matrix setup
	float epsilon = 0.01f;
	if (camera.IsPerspective && mixValue > epsilon) {
		mixValue -= deltaTime * 5;
		if (mixValue <= epsilon) {
			mixValue = 0.0f;
		}
	}
	else if (!camera.IsPerspective && mixValue < 1 - epsilon) {
		mixValue += deltaTime * 5;
		if (mixValue >= 1 - epsilon) {
			mixValue = 1.0f;
		}
	}
	glm::mat4 projection = lerpProjectionMatrices(projectionPerspective, projectionOrtho, mixValue);

	//////////////////////////////////////////////////////////////
	// Lights setup in shader
	glm::vec3 emptyVec3(0.0f, 0.0f, 0.0f);

	Shader& shader_texture_phong_materials = shaders.find("shader_texture_phong_materials")->second;
	shader_texture_phong_materials.use();
	shader_texture_phong_materials.setMatrixFloat4v("view", 1, view);
	shader_texture_phong_materials.setMatrixFloat4v("projection", 1, projection);
	shader_texture_phong_materials.setVec3("viewPosition", camera.Position);

	if (directionalLight.Enabled) {
		shader_texture_phong_materials.setVec3("directionalLight.direction", directionalLight.Direction);
		shader_texture_phong_materials.setVec3("directionalLight.ambient", directionalLight.Ambient);
		shader_texture_phong_materials.setVec3("directionalLight.diffuse", directionalLight.Diffuse);
		shader_texture_phong_materials.setVec3("directionalLight.specular", directionalLight.Specular);
	}
	else {
		shader_texture_phong_materials.setVec3("directionalLight.direction", emptyVec3);
		shader_texture_phong_materials.setVec3("directionalLight.ambient", emptyVec3);
		shader_texture_phong_materials.setVec3("directionalLight.diffuse", emptyVec3);
		shader_texture_phong_materials.setVec3("directionalLight.specular", emptyVec3);
	}

	for (int i = 0; i < pointLights.size(); ++i) {
		std::string index = std::to_string(i);
		const auto& pointLight = pointLights[i];
		if (pointLight.Enabled) {
			shader_texture_phong_materials.setVec3("pointLights[" + index + "].position", pointLight.Position);
			shader_texture_phong_materials.setFloat("pointLights[" + index + "].constant", pointLight.Constant);
			shader_texture_phong_materials.setFloat("pointLights[" + index + "].linear", pointLight.Linear);
			shader_texture_phong_materials.setFloat("pointLights[" + index + "].quadratic", pointLight.Quadratic);
			shader_texture_phong_materials.setVec3("pointLights[" + index + "].ambient", pointLight.Ambient);
			shader_texture_phong_materials.setVec3("pointLights[" + index + "].diffuse", pointLight.Diffuse);
			shader_texture_phong_materials.setVec3("pointLights[" + index + "].specular", pointLight.Specular);
		}
		else {
			shader_texture_phong_materials.setVec3("pointLights[" + index + "].position", emptyVec3);
			shader_texture_phong_materials.setFloat("pointLights[" + index + "].constant", 0.0f);
			shader_texture_phong_materials.setFloat("pointLights[" + index + "].linear", 0.0f);
			shader_texture_phong_materials.setFloat("pointLights[" + index + "].quadratic", 0.0f);
			shader_texture_phong_materials.setVec3("pointLights[" + index + "].ambient", emptyVec3);
			shader_texture_phong_materials.setVec3("pointLights[" + index + "].diffuse", emptyVec3);
			shader_texture_phong_materials.setVec3("pointLights[" + index + "].specular", emptyVec3);
		}
	}

	for (int i = 0; i < spotLights.size(); ++i) {
		std::string index = std::to_string(i);
		const auto& spotLight = spotLights[i];
		if (spotLight.Enabled) {
			shader_texture_phong_materials.setVec3("spotLights[" + index + "].position", spotLight.Position);
			shader_texture_phong_materials.setVec3("spotLights[" + index + "].direction", spotLight.Direction);
			shader_texture_phong_materials.setFloat("spotLights[" + index + "].innerCutOff", spotLight.InnerCutOff);
			shader_texture_phong_materials.setFloat("spotLights[" + index + "].outerCutOff", spotLight.OuterCutOff);
			shader_texture_phong_materials.setFloat("spotLights[" + index + "].constant", spotLight.Constant);
			shader_texture_phong_materials.setFloat("spotLights[" + index + "].linear", spotLight.Linear);
			shader_texture_phong_materials.setFloat("spotLights[" + index + "].quadratic", spotLight.Quadratic);
			shader_texture_phong_materials.setVec3("spotLights[" + index + "].ambient", spotLight.Ambient);
			shader_texture_phong_materials.setVec3("spotLights[" + index + "].diffuse", spotLight.Diffuse);
			shader_texture_phong_materials.setVec3("spotLights[" + index + "].specular", spotLight.Specular);
		}
		else {
			shader_texture_phong_materials.setVec3("spotLights[" + index + "].position", emptyVec3);
			shader_texture_phong_materials.setVec3("spotLights[" + index + "].direction", emptyVec3);
			shader_texture_phong_materials.setFloat("spotLights[" + index + "].innerCutOff", 0.0f);
			shader_texture_phong_materials.setFloat("spotLights[" + index + "].outerCutOff", 0.0f);
			shader_texture_phong_materials.setFloat("spotLights[" + index + "].constant", 0.0f);
			shader_texture_phong_materials.setFloat("spotLights[" + index + "].linear", 0.0f);
			shader_texture_phong_materials.setFloat("spotLights[" + index + "].quadratic", 0.0f);
			shader_texture_phong_materials.setVec3("spotLights[" + index + "].ambient", emptyVec3);
			shader_texture_phong_materials.setVec3("spotLights[" + index + "].diffuse", emptyVec3);
			shader_texture_phong_materials.setVec3("spotLights[" + index + "].specular", emptyVec3);
		}
	}

	Shader& shader_color_phong_materials = shaders.find("shader_color_phong_materials")->second;
	shader_color_phong_materials.use();
	shader_color_phong_materials.setMatrixFloat4v("view", 1, view);
	shader_color_phong_materials.setMatrixFloat4v("projection", 1, projection);
	shader_color_phong_materials.setVec3("viewPosition", camera.Position);

	if (directionalLight.Enabled) {
		shader_color_phong_materials.setVec3("directionalLight.direction", directionalLight.Direction);
		shader_color_phong_materials.setVec3("directionalLight.ambient", directionalLight.Ambient);
		shader_color_phong_materials.setVec3("directionalLight.diffuse", directionalLight.Diffuse);
		shader_color_phong_materials.setVec3("directionalLight.specular", directionalLight.Specular);
	}
	else {
		shader_color_phong_materials.setVec3("directionalLight.direction", emptyVec3);
		shader_color_phong_materials.setVec3("directionalLight.ambient", emptyVec3);
		shader_color_phong_materials.setVec3("directionalLight.diffuse", emptyVec3);
		shader_color_phong_materials.setVec3("directionalLight.specular", emptyVec3);
	}

	for (int i = 0; i < pointLights.size(); ++i) {
		std::string index = std::to_string(i);
		const auto& pointLight = pointLights[i];
		if (pointLight.Enabled) {
			shader_color_phong_materials.setVec3("pointLights[" + index + "].position", pointLight.Position);
			shader_color_phong_materials.setFloat("pointLights[" + index + "].constant", pointLight.Constant);
			shader_color_phong_materials.setFloat("pointLights[" + index + "].linear", pointLight.Linear);
			shader_color_phong_materials.setFloat("pointLights[" + index + "].quadratic", pointLight.Quadratic);
			shader_color_phong_materials.setVec3("pointLights[" + index + "].ambient", pointLight.Ambient);
			shader_color_phong_materials.setVec3("pointLights[" + index + "].diffuse", pointLight.Diffuse);
		}
		else {
			shader_color_phong_materials.setVec3("pointLights[" + index + "].specular", emptyVec3);
			shader_color_phong_materials.setVec3("pointLights[" + index + "].position", emptyVec3);
			shader_color_phong_materials.setFloat("pointLights[" + index + "].constant", 0.0f);
			shader_color_phong_materials.setFloat("pointLights[" + index + "].linear", 0.0f);
			shader_color_phong_materials.setFloat("pointLights[" + index + "].quadratic", 0.0f);
			shader_color_phong_materials.setVec3("pointLights[" + index + "].ambient", emptyVec3);
			shader_color_phong_materials.setVec3("pointLights[" + index + "].diffuse", emptyVec3);
			shader_color_phong_materials.setVec3("pointLights[" + index + "].specular", emptyVec3);
		}
	}

	for (int i = 0; i < spotLights.size(); ++i) {
		std::string index = std::to_string(i);
		const auto& spotLight = spotLights[i];
		if (spotLight.Enabled) {
			shader_color_phong_materials.setVec3("spotLights[" + index + "].position", spotLight.Position);
			shader_color_phong_materials.setVec3("spotLights[" + index + "].direction", spotLight.Direction);
			shader_color_phong_materials.setFloat("spotLights[" + index + "].innerCutOff", spotLight.InnerCutOff);
			shader_color_phong_materials.setFloat("spotLights[" + index + "].outerCutOff", spotLight.OuterCutOff);
			shader_color_phong_materials.setFloat("spotLights[" + index + "].constant", spotLight.Constant);
			shader_color_phong_materials.setFloat("spotLights[" + index + "].linear", spotLight.Linear);
			shader_color_phong_materials.setFloat("spotLights[" + index + "].quadratic", spotLight.Quadratic);
			shader_color_phong_materials.setVec3("spotLights[" + index + "].ambient", spotLight.Ambient);
			shader_color_phong_materials.setVec3("spotLights[" + index + "].diffuse", spotLight.Diffuse);
		}
		else {
			shader_color_phong_materials.setVec3("spotLights[" + index + "].specular", emptyVec3);
			shader_color_phong_materials.setVec3("spotLights[" + index + "].position", emptyVec3);
			shader_color_phong_materials.setVec3("spotLights[" + index + "].direction", emptyVec3);
			shader_color_phong_materials.setFloat("spotLights[" + index + "].innerCutOff", 0.0f);
			shader_color_phong_materials.setFloat("spotLights[" + index + "].outerCutOff", 0.0f);
			shader_color_phong_materials.setFloat("spotLights[" + index + "].constant", 0.0f);
			shader_color_phong_materials.setFloat("spotLights[" + index + "].linear", 0.0f);
			shader_color_phong_materials.setFloat("spotLights[" + index + "].quadratic", 0.0f);
			shader_color_phong_materials.setVec3("spotLights[" + index + "].ambient", emptyVec3);
			shader_color_phong_materials.setVec3("spotLights[" + index + "].diffuse", emptyVec3);
			shader_color_phong_materials.setVec3("spotLights[" + index + "].specular", emptyVec3);
		}
	}

	// Declared early that way I'm sure it exists
	// Should still reset it every time though
	glm::mat4 model = glm::mat4(1.0f);

	//////////////////////////////////////////////////////////////
	// Render OpenGL
	if (drawPlane) {
		glBindVertexArray(VAO[0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_container);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);

		model = glm::mat4(1.0f);
		model = glm::translate(model, planePosition);
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 1.0f));

		shader_texture_phong_materials.use();
		shader_texture_phong_materials.setMatrixFloat4v("model", 1, model);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// TODO: unbind/deactivate textures?
		// TODO: functionize
		Shader::release();
		glBindVertexArray(0);
	}
	if (drawTexturedCubes) {
		shader_texture_phong_materials.use();
		shader_texture_phong_materials.setFloat("material.shininess", (float)shininess);

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

		glBindVertexArray(VAO[1]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_container2);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture_container2Specular);

		for (int i = 0; i < 10; ++i) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			//model = glm::rotate(model, (float)glfwGetTime() * glm::radians(30.0f) + 30 * i, glm::vec3(1.0f, 0.0f, 0.0f));
			//model = glm::rotate(model, (float)glfwGetTime() * glm::radians(20.0f) + 20 * i, glm::vec3(0.0f, 1.0f, 0.0f));
			//model = glm::rotate(model, (float)glfwGetTime() * glm::radians(10.0f) + 10 * i, glm::vec3(0.0f, 0.0f, 1.0f));
			shader_texture_phong_materials.setMatrixFloat4v("model", 1, model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 2.0f, -5.0f));
		shader_texture_phong_materials.setMatrixFloat4v("model", 1, model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		////glActiveTexture(GL_TEXTURE2);
		////glBindTexture(GL_TEXTURE_2D, 0);

		Shader::release();
		glBindVertexArray(0);
	}
	if (drawMaterialCubes) {
		// Diffuse map / Specular map cube
		glBindVertexArray(VAO[1]);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.0f, 2.0f, -5.0f));
		//model = glm::rotate(model, (float)glfwGetTime() * glm::radians(30.0f) + 30 * i, glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, (float)glfwGetTime() * glm::radians(20.0f) + 20 * i, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, (float)glfwGetTime() * glm::radians(10.0f) + 10 * i, glm::vec3(0.0f, 0.0f, 1.0f));

		//glActiveTexture(GL_TEXTURE2);
		//glBindTexture(GL_TEXTURE_2D, texture_matrix);

		shader_color_phong_materials.use();
		shader_color_phong_materials.setMatrixFloat4v("model", 1, model);
		shader_color_phong_materials.setVec3("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
		shader_color_phong_materials.setVec3("material.specular", glm::vec3(1.0f, 0.5f, 0.31f));
		shader_color_phong_materials.setVec3("material.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
		shader_color_phong_materials.setFloat("material.shininess", (float)shininess);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		Shader::release();
		glBindVertexArray(0);
	}
	if (drawLights) {
		glBindVertexArray(VAO[1]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_redstoneLamp);

		Shader& shader_texture_simple = shaders.find("shader_texture_simple")->second;
		shader_texture_simple.use();
		shader_texture_simple.setMatrixFloat4v("view", 1, view);
		shader_texture_simple.setMatrixFloat4v("projection", 1, projection);

		for (auto const& pointLight : pointLights) {
			if (pointLight.Enabled && pointLight.Visible) {
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, pointLight.Position);
				model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
				shader_texture_simple.setMatrixFloat4v("model", 1, model);

				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}

		for (auto const& spotLight : spotLights) {
			if (spotLight.Enabled && spotLight.Visible) {
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, spotLight.Position);
				model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
				// TODO
				//model = glm::rotate(model, (float)glm::radians(glfwGetTime()), spotLight.Direction);
				//glm::lookAt(spotLight.Position, spotLight.Direction, glm::vec3(0.0f, 1.0f, 0.0f));
				shader_texture_simple.setMatrixFloat4v("model", 1, model);

				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}

		Shader::release();
		glBindVertexArray(0);
	}

	// gizmo
	if (drawGizmo) {
		glViewport(10.0f, 10.0f, 100.0f, 100.0f);
		glClear(GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(VAOGizmo[0]);

		Shader& shader_color_uniform = shaders.find("shader_color_uniform")->second;
		shader_color_uniform.use();

		// Fixed postition so that camera position doesn't change render 
		glm::mat4 viewGizmo = glm::mat4(view);
		viewGizmo[3][0] = 0.0f;
		viewGizmo[3][1] = 0.0f;
		viewGizmo[3][2] = -2.5f;
		shader_color_uniform.setMatrixFloat4v("view", 1, viewGizmo);

		glm::mat4 projectionGizmo = lerpProjectionMatrices(projectionPerspectiveGizmo, projectionOrthoGizmo, mixValue);

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
		shader_color_uniform.setFloat("shininess", (float)shininess);

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
		glBindVertexArray(0);

		glViewport(0, 0, mode->width, mode->height);
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

	if (ImGui::CollapsingHeader("Lights", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (ImGui::TreeNodeEx("Directional Light", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Checkbox("Enabled", &directionalLight.Enabled);
			ImGui::Checkbox("Visible", &directionalLight.Visible);

			ImGui::DragFloat3("Direction", &directionalLight.Direction[0], 0.1f, -1.0f, 1.0f);

			ImGui::ColorEdit3("Ambient", &directionalLight.Ambient[0], ImGuiColorEditFlags_Float);
			ImGui::ColorEdit3("Diffuse", &directionalLight.Diffuse[0], ImGuiColorEditFlags_Float);
			ImGui::ColorEdit3("Specular", &directionalLight.Specular[0], ImGuiColorEditFlags_Float);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Point Lights")) {
			// TODO: name
			for (int i = 0; i < pointLights.size(); ++i) {
				PointLight& pointLight = pointLights[i];
				std::string text("Point Light [" + std::to_string(i) + "]");
				if (ImGui::TreeNodeEx(text.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
					ImGui::Checkbox("Enabled", &pointLight.Enabled);
					ImGui::Checkbox("Visible", &pointLight.Visible);

					ImGui::DragFloat3("Position", &pointLight.Position[0], 0.1f, -10.0f, 10.0f);

					ImGui::DragFloat("Constant", &pointLight.Constant, 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat("Linear", &pointLight.Linear, 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat("Quadratic", &pointLight.Quadratic, 0.01f, 0.0f, 1.0f);

					ImGui::ColorEdit3("Ambient", &pointLight.Ambient[0], ImGuiColorEditFlags_Float);
					ImGui::ColorEdit3("Diffuse", &pointLight.Diffuse[0], ImGuiColorEditFlags_Float);
					ImGui::ColorEdit3("Specular", &pointLight.Specular[0], ImGuiColorEditFlags_Float);
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Spot Light")) {
			for (int i = 0; i < spotLights.size(); ++i) {
				SpotLight& spotLight = spotLights[i];
				std::string text("Spot Light [" + std::to_string(i) + "]");
				if (ImGui::TreeNodeEx(text.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
					ImGui::Checkbox("Enabled", &spotLight.Enabled);
					ImGui::Checkbox("Visible", &spotLight.Visible);

					ImGui::DragFloat3("Position", &spotLight.Position[0], 0.1f, -10.0f, 10.0f);
					ImGui::DragFloat3("Direction", &spotLight.Direction[0], 0.1f, -1.0f, 1.0f);

					float tempInnerCutOff(glm::degrees(glm::acos(spotLight.InnerCutOff)));
					float tempOuterCutOff(glm::degrees(glm::acos(spotLight.OuterCutOff)));
					ImGui::DragFloat("Inner Cut Off", &tempInnerCutOff, 0.5f, 0.1f, 45.0f);
					ImGui::DragFloat("Outer Cut Off", &tempOuterCutOff, 0.5f, 0.1f, 45.0f);
					spotLight.InnerCutOff = glm::cos(glm::radians(tempInnerCutOff));
					spotLight.OuterCutOff = glm::cos(glm::radians(tempOuterCutOff));

					ImGui::DragFloat("Constant", &spotLight.Constant, 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat("Linear", &spotLight.Linear, 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat("Quadratic", &spotLight.Quadratic, 0.01f, 0.0f, 1.0f);

					ImGui::ColorEdit3("Ambient", &spotLight.Ambient[0], ImGuiColorEditFlags_Float);
					ImGui::ColorEdit3("Diffuse", &spotLight.Diffuse[0], ImGuiColorEditFlags_Float);
					ImGui::ColorEdit3("Specular", &spotLight.Specular[0], ImGuiColorEditFlags_Float);
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
	}

	if (ImGui::CollapsingHeader("Draws", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Checkbox("Draw Plane?", &drawPlane);
		ImGui::Checkbox("Draw Textured Cubes?", &drawTexturedCubes);
		ImGui::Checkbox("Draw Material Cubes?", &drawMaterialCubes);
		ImGui::Checkbox("Draw Lights?", &drawLights);

		ImGui::SliderFloat("Mix Value", &mixValue, 0.0f, 1.0f);

		ImGui::DragFloat3("Plane position", &planePosition[0], 0.1f, -10.0f, 10.0f);

		ImGui::Separator();
		ImGui::SliderFloat("FOV", &camera.FOV, 10.0f, 180.0f);
		ImGui::Checkbox("Is perspective?", &camera.IsPerspective);
		//ImGui::DragFloat4("Ortho (L,R,B,T)", &camera.OrthographicFactor[0], 0.1f, -20.0f, 20.0f);
		ImGui::SliderFloat("Ortho Factor", &camera.OrthographicFactor, 1.0f, 10.0f);
		ImGui::DragFloat("Ortho Factorr", &camera.OrthographicFactor, 0.1f, 1.0f, 10.0f);
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
	glDeleteTextures(5, &textures[0]);

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
	bool isMiddleMousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_3);

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

	if (isMiddleMousePressed && !lastMiddleClick) {
		// Just pressed middle click
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		firstMouse = true;
	}
	else if (!isMiddleMousePressed && lastMiddleClick) {
		// Just released middle click
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	// Additional checks to ignore clicks on ImGui windows
	if (isRightMousePressed && lastRightClick && !ImGui::IsAnyWindowHovered() && !ImGui::IsAnyItemHovered()) {
		camera.processMouseMovement(deltaTime, deltaX, deltaY);
	}

	// Additional checks to ignore clicks on ImGui windows
	if (isMiddleMousePressed && lastMiddleClick && !ImGui::IsAnyWindowHovered() && !ImGui::IsAnyItemHovered()) {
		camera.processMouseMovement2(deltaTime, deltaX, deltaY);
	}

	lastMousePosX = currentMousePosX;
	lastMousePosY = currentMousePosY;

	lastLeftClick = isLeftMousePressed;
	lastRightClick = isRightMousePressed;
	lastMiddleClick = isMiddleMousePressed;

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

	// VSYNC
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS && keys[GLFW_KEY_V] == GLFW_RELEASE) {
		vsync = !vsync;
		glfwSwapInterval(vsync ? 1 : 0);
	}

	// FULLSCREEN / WINDOWED FULLSCREEN
	// https://www.glfw.org/docs/latest/window.html#window_windowed_full_screen
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && keys[GLFW_KEY_F] == GLFW_RELEASE) {
		fullscreen = !fullscreen;

		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		if (fullscreen) {
			glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), NULL, NULL, mode->width, mode->height, mode->refreshRate);
		}
		else {
			glfwSetWindowMonitor(window, nullptr, (mode->width - width) / 2, (mode->height - height) / 2, width, height, mode->refreshRate);
		}

		// Because going to fullscreen apparently leads to no FPS cap?
		glfwSwapInterval(vsync ? 1 : 0);
	}

	// Update states
	keys[GLFW_KEY_Z] = glfwGetKey(window, GLFW_KEY_Z);
	keys[GLFW_KEY_F] = glfwGetKey(window, GLFW_KEY_F);
	keys[GLFW_KEY_V] = glfwGetKey(window, GLFW_KEY_V);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (!ImGui::IsAnyWindowHovered() && !ImGui::IsAnyItemHovered()) {
		camera.processMouseScroll(yoffset * scrollSpeed);
	}
}