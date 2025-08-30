#pragma once
#define USE_STD_FILESYSTEM
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ImGuiFileDialog.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>

#include "Light.h"

// Forward declare the UI class
class UI;

struct UIParams {
	int renderMode;
	int polygonMode;
	bool drawGrass;
	bool drawNormals;
	float normalsColor[4];
	float normalLength;
	float modelRotation[3];
	bool showLightPositions;
	std::vector<Light>* lightsVec;
	std::function<void(const std::string&)> callback; // Use std::function directly;
};

class UI
{
public:
	UI(GLFWwindow* window, UIParams& params);
	~UI();

	void Setup();
	void Render();
	void Shutdown();

private:
	GLFWwindow* m_window;
	UIParams& m_uiParams;

	void DrawImgui();
};
