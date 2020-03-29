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