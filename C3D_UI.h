#ifndef C3D_UI_H
#define C3D_UI_H
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class UI
{
public:
	UI(GLFWwindow* window, bool& drawNormals, int& renderMode, int& polygonMode, float (&normalsColor)[4]);
	~UI();

	void Setup();
	void Render();
	void Shutdown();

private:
	GLFWwindow* m_Window;
	bool* m_DrawNormals;
	int* m_RenderMode;
	int* m_PolygonMode;
	float* m_NormalsColor[4];

	void DrawImgui();
};

#endif