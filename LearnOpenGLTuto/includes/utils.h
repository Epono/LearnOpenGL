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

void showImguiDemo()
{
	static int counter = 0;

	// position the controls widget in the top-right corner with some margin
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_FirstUseEver);
	// create the window and append into it
	ImGui::Begin("Info", nullptr, ImGuiWindowFlags_None);

	ImGui::Dummy(ImVec2(0.0f, 1.0f));
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Platform");
	ImGui::Text("%s", "Windows lol");
	ImGui::Text("CPU cores: %d", std::thread::hardware_concurrency());
	ImGui::Text("RAM: %.2f GB", "SDL_GetSystemRAM() / 1024.0f");

	// buttons and most other widgets return true when clicked/edited/activated
	if (ImGui::Button("Counter button"))
	{
		counter++;
	}
	ImGui::SameLine();
	ImGui::Text("counter = %d", counter);
	static float v[5] = { 0.390f, 0.575f, 0.565f, 1.000f };
	ImGui::Bezier("easeInOutQuint", v);       // draw
	float y = ImGui::BezierValue(0.5f, v); // x delta in [0..1] range
	ImGui::End();
}

GLsizei gizmoWidth = 100.0f;
GLsizei gizmoHeight = 100.0f;

void processInput(GLFWwindow* window, double deltaTime);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void createOpenGLObjects();
void cleanUp();
void createTextures();
void createShaders();
void render(double deltaTime);
void update(double deltaTime);
unsigned int createTexture(const std::string& texturePath);

inline float B0(float t) { return t * t * t; }
inline float B1(float t) { return 3 * t * t * (1 - t); }
inline float B2(float t) { return 3 * t * (1 - t) * (1 - t); }
inline float B3(float t) { return (1 - t) * (1 - t) * (1 - t); }

glm::vec2 getBezier(float t, const glm::vec2& P0, const glm::vec2& P1, const glm::vec2& P2, const glm::vec2& P3) {
	return glm::vec2(
		P1.x * B0(t) + P1.x * B1(t) + P2.x * B2(t) + P3.x * B3(t),
		P1.y * B0(t) + P1.y * B1(t) + P2.y * B2(t) + P3.y * B3(t)
	);
}

float getBezierSimplified(float t, const glm::vec2& P1, const glm::vec2& P2) {
	return getBezier(t, glm::vec2(0.0f, 0.0f), P1, P2, glm::vec2(0.0f, 0.0f)).y;
}

glm::mat4 projectionPerspectiveGizmo = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
glm::mat4 projectionOrthoGizmo = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);

glm::mat4 lerpProjectionMatrices(const glm::mat4& perspective, const glm::mat4& ortho, float t) {
	glm::mat4 resultMatrix = glm::mat4(1.0f);
	float mixProjections = getBezierSimplified(t, glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 1.0f));
	resultMatrix[0] = glm::mix(perspective[0], ortho[0], mixProjections);
	resultMatrix[1] = glm::mix(perspective[1], ortho[1], mixProjections);
	resultMatrix[2] = glm::mix(perspective[2], ortho[2], mixProjections);
	resultMatrix[3] = glm::mix(perspective[3], ortho[3], mixProjections);
	return resultMatrix;
}


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

float verticesLine[] = {
	// position
	 0.0f, 0.0f, 0.0f,
	 0.9f, 0.0f, 0.0f
};


unsigned int createTexture(const std::string& texturePath) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	// load and generate texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		GLenum format;
		if (nrChannels == 1) {
			format = GL_RED;
		}
		else if (nrChannels == 3) {
			format = GL_RGB;
		}
		else if (nrChannels == 4) {
			format = GL_RGBA;
		}
		else {
			std::cout << "Weird number of channels for texture [" << texturePath << "]: " << nrChannels << std::endl;
			// TODO:
		}
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// set texture wrapping/filtering options on currently bound texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	return textureID;
}

// Too much "callbacky", better to manually check each frame?
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

/*
//https://gamedev.stackexchange.com/questions/43691/how-can-i-move-an-object-in-an-infinity-or-figure-8-trajectory
float t = glfwGetTime();
float scale = 2 / (3 - cos(2 * t));
float lightPositionOffsetX = 3 * scale * cos(t);
float lightPositionOffsetY = 3 * sin(t);
float lightPositionOffsetZ = 6 * scale * sin(t * 2) / 2;
*/