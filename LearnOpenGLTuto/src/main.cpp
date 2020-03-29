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

#include <stb_image/stb_image.h>

#include <iostream>
#include <vector>
#include <map>
#include <thread>

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

float redValue = 1.0f;
float greenValue = 1.0f;
float blueValue = 1.0f;

float keyRepeatDelay = 0.5f;
float keyRepeatSpacing = 0.05f;
bool keys[350] = { false };

void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void createOpenGLObjects();
void cleanUp();
void render(const double currentTime);

int width = 800;
int height = 600;

unsigned int VAO[5], VBO[4], EBO[5];
std::map<std::string, Shader> shaders;

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	std::unique_ptr<GLFWwindow, glfwDeleter> window;
	window.reset(glfwCreateWindow(width, height, "LearnOpenGL", nullptr, nullptr));

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

	// Viewport inside the window
	// Can spill out ouf window
	// If smaller than window, takes only a fraction of the window
	glViewport(0, 0, width, height);

	// Callbacks
	glfwSetFramebufferSizeCallback(window.get(), framebuffer_size_callback);

	Shader default_shader("shaders/shader.vert", "shaders/shader.frag");
	Shader shader_uniform("shaders/shader_uniform.vert", "shaders/shader_uniform.frag");
	Shader shader_color_attribute("shaders/shader_color_attribute.vert", "shaders/shader_color_attribute.frag");

	shaders.insert(std::make_pair("default_shader", default_shader));
	shaders.insert(std::make_pair("shader_uniform", shader_uniform));
	shaders.insert(std::make_pair("shader_color_attribute", shader_color_attribute));

	createOpenGLObjects();


	// setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigWindowsResizeFromEdges = true;
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//io.ConfigDockingWithShift = false;

	// setup Dear ImGui style
	ImGui::StyleColorsDark();

	// setup platform/renderer bindings
	std::string glsl_version = "#version 330";
	ImGui_ImplGlfw_InitForOpenGL(window.get(), true);
	ImGui_ImplOpenGL3_Init(glsl_version.c_str());


	double previousTime = glfwGetTime();

	// Main loop
	while (!glfwWindowShouldClose(window.get())) {
		double currentTime = glfwGetTime();

		double deltaTime = currentTime - previousTime;
		previousTime = currentTime;
		//std::cout << elapsed << " s - FPS: " << 1 / elapsed  << std::endl;

		// input
		processInput(window.get());

		// render
		render(currentTime);

		// check and call events and swap the buffers
		glfwPollEvents();
		glfwSwapBuffers(window.get());
	}

	cleanUp();

	return 0;
}

float greenValue2 = 1.0f;
ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
bool drawF = true, drawGayTriangle = true, drawRectangle = true, drawTexturedTriangle = true;

void render(const double currentTime) {
	glClearColor(redValue, greenValue, blueValue, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	if (drawRectangle) {
		// Rectangle
		shaders.find("default_shader")->second.use();
		glBindVertexArray(VAO[0]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glBindVertexArray(0);
	}

	if (drawGayTriangle) {
		// Bottom Triangle
		shaders.find("shader_color_attribute")->second.use();
		glBindVertexArray(VAO[1]);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		//glBindVertexArray(0);
	}

	if (drawF) {
		// Red F
		shaders.find("default_shader")->second.use();
		glBindVertexArray(VAO[2]);
		glDrawElements(GL_TRIANGLES, 11, GL_UNSIGNED_INT, 0);
		//glBindVertexArray(0);

		// Green F
		shaders.find("shader_uniform")->second.use();
		/*float greenValue2 = (sin(currentTime) / 2.0f) + 0.5f;*/
		shaders.find("shader_uniform")->second.setFloat4("ourColor", 0.0f, greenValue2, 0.0f, 1.0f);
		glBindVertexArray(VAO[3]);
		glDrawElements(GL_TRIANGLES, 11, GL_UNSIGNED_INT, 0);
		//glBindVertexArray(0);
	}

	if (drawTexturedTriangle) {
		// Textured Triangle
		shaders.find("shader_color_attribute")->second.use();
		glBindVertexArray(VAO[4]);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		//glBindVertexArray(0);
	}

	glBindVertexArray(0);



	// start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// a window is defined by Begin/End pair
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



		ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_FirstUseEver);
		// create the window and append into it
		ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_None);

		if (ImGui::CollapsingHeader("Colors"))
		{
			ImGui::SliderFloat("Green value", &greenValue2, 0.0f, 1.0f);
			ImGui::Separator();

			ImGui::ColorEdit4("Background color", (float*)&color, ImGuiColorEditFlags_Float);
			redValue = color.x;
			greenValue = color.y;
			blueValue = color.z;
		}

		if (ImGui::CollapsingHeader("Draws", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Checkbox("Draw F?", &drawF);
			ImGui::Checkbox("Draw Gay Triangle?", &drawGayTriangle);
			ImGui::Checkbox("Draw Rectangle?", &drawRectangle);
		}
		ImGui::End();
	}

	// rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void cleanUp() {
	glDeleteVertexArrays(3, VAO);
	glDeleteBuffers(2, VBO);
	glDeleteBuffers(3, EBO);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void createOpenGLObjects() {
	glGenVertexArrays(5, VAO);
	glGenBuffers(4, VBO);
	glGenBuffers(5, EBO);

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





	// TRIANGLE
	float verticesBigTriangle[] = {
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		 0.0f,  0.5f, 0.0f,  // top 
	};

	unsigned int indicesBigTriangle[] = {
		0, 1, 2,
	};

	float textCoords[] = {
		0.0f, 0.0f, // lower-left corner
		1.0f, 0.0f, // lower-right corner
		0.5f, 1.0f  // top-center corner
	};

	glBindVertexArray(VAO[4]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesBigTriangle), verticesBigTriangle, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[4]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesBigTriangle), indicesBigTriangle, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float) /* could be 0 too */, (void*)0);
	glEnableVertexAttribArray(0);




	// Unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	// Backgopund color
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

	// Update states
	keys[GLFW_KEY_Z] = glfwGetKey(window, GLFW_KEY_Z);
	keys[GLFW_KEY_KP_7] = glfwGetKey(window, GLFW_KEY_KP_7);
	keys[GLFW_KEY_KP_4] = glfwGetKey(window, GLFW_KEY_KP_4);
	keys[GLFW_KEY_KP_8] = glfwGetKey(window, GLFW_KEY_KP_8);
	keys[GLFW_KEY_KP_5] = glfwGetKey(window, GLFW_KEY_KP_5);
	keys[GLFW_KEY_KP_9] = glfwGetKey(window, GLFW_KEY_KP_9);
	keys[GLFW_KEY_KP_6] = glfwGetKey(window, GLFW_KEY_KP_6);
}

// Too much "callbacky", better to manually check each frame?
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}