#include <utils.h>



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
	//static float v[5] = { 0.390f, 0.575f, 0.565f, 1.000f };
	//ImGui::Bezier("easeInOutQuint", v);       // draw
	//float y = ImGui::BezierValue(0.5f, v); // x delta in [0..1] range
	ImGui::End();
}

unsigned int createTexture(const std::string& folderPath, const std::string& name, bool gamma) {

	std::string filename(folderPath + "/" + name);

	unsigned int textureID;
	glGenTextures(1, &textureID);

	// load and generate texture
	int width, height, channelsNumber;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channelsNumber, 0);
	if (data) {
		GLenum format;
		if (channelsNumber == 1) {
			format = GL_RED;
		}
		else if (channelsNumber == 3) {
			format = GL_RGB;
		}
		else if (channelsNumber == 4) {
			format = GL_RGBA;
		}
		else {
			std::cout << "Weird number of channels for texture [" << name << "]: " << channelsNumber << std::endl;
			// TODO:
		}
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// set texture wrapping/filtering options on currently bound texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	return textureID;
}

glm::vec2 getBezier(float t, const glm::vec2& P0, const glm::vec2& P1, const glm::vec2& P2, const glm::vec2& P3) {
	return glm::vec2(
		P1.x * B0(t) + P1.x * B1(t) + P2.x * B2(t) + P3.x * B3(t),
		P1.y * B0(t) + P1.y * B1(t) + P2.y * B2(t) + P3.y * B3(t)
	);
}

float getBezierSimplified(float t, const glm::vec2& P1, const glm::vec2& P2) {
	return getBezier(t, glm::vec2(0.0f, 0.0f), P1, P2, glm::vec2(0.0f, 0.0f)).y;
}


glm::mat4 lerpProjectionMatrices(const glm::mat4& perspective, const glm::mat4& ortho, float t) {
	glm::mat4 resultMatrix = glm::mat4(1.0f);
	float mixProjections = getBezierSimplified(t, glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 1.0f));
	resultMatrix[0] = glm::mix(perspective[0], ortho[0], mixProjections);
	resultMatrix[1] = glm::mix(perspective[1], ortho[1], mixProjections);
	resultMatrix[2] = glm::mix(perspective[2], ortho[2], mixProjections);
	resultMatrix[3] = glm::mix(perspective[3], ortho[3], mixProjections);
	return resultMatrix;
}