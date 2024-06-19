#define USE_STD_FILESYSTEM
#ifndef C3D_UI_H
#define C3D_UI_H
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ImGuiFileDialog.h"
#include "glm/glm.hpp"
#include <iostream>

class UI
{
public:
	using FileSelectedCallback = std::function<void(const std::string&)>;

	UI(GLFWwindow* window, bool& drawNormals, int& renderMode, int& polygonMode, float(&normalsColor)[4], float& modelRotation, float& normalLength, FileSelectedCallback callback);
	~UI();

	void Setup();
	void Render();
	void Shutdown();

private:
	GLFWwindow* m_window;
	FileSelectedCallback m_callback;

	int*   m_renderMode;
	int*   m_polygonMode;
	bool*  m_drawNormals;
	float* m_modelRotation;
	float* m_normalsColor[4];
	float* m_normalLen;

	void DrawImgui();
};

#endif