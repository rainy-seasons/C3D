#define USE_STD_FILESYSTEM
#ifndef C3D_UI_H
#define C3D_UI_H
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ImGuiFileDialog.h"
#include "glm/glm.hpp"
#include <iostream>

// Forward declare the UI class
class UI;

struct UIParams {
	GLFWwindow* window;
	int& renderMode;
	int& polygonMode;
	bool& drawGrass;
	bool& drawNormals;
	float(&normalsColor)[4];
	float& normalLength;
	float(&modelRotation)[3];
	std::function<void(const std::string&)> callback; // Use std::function directly;
};

class UI
{
public:
	using FileSelectedCallback = std::function<void(const std::string&)>;

	UI(const UIParams& params);
	~UI();

	void Setup();
	void Render();
	void Shutdown();

private:
	GLFWwindow* m_window;
	FileSelectedCallback m_callback;

	int*   m_renderMode;
	int*   m_polygonMode;
	bool*  m_drawGrass;
	bool*  m_drawNormals;
	float* m_normalLen;
	float* m_normalsColor[4];
	float* m_modelRotation[3];

	void DrawImgui();
};

#endif
