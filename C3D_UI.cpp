#include "C3D_UI.h"

UI::UI(GLFWwindow* window, bool& drawNormals, int& renderMode, int& polygonMode, float (&normalsColor)[4])
{
	this->m_Window = window;
	this->m_DrawNormals = &drawNormals;
	this->m_RenderMode = &renderMode;
	this->m_PolygonMode = &polygonMode;

	// TODO: fix this ugly shit
	this->m_NormalsColor[0] = &normalsColor[0];
	this->m_NormalsColor[1] = &normalsColor[1];
	this->m_NormalsColor[2] = &normalsColor[2];
	this->m_NormalsColor[3] = &normalsColor[3];
}

UI::~UI()
{
	Shutdown();
}

void UI::Setup()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void UI::Render()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	DrawImgui();
	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::DrawImgui()
{
	ImGui::Begin("C3D");

	ImGui::Combo("Render Mode", m_RenderMode, "Depth (Textureless)\0Depth (Textured)\0Direct Light\0Point Light\0Spot Light\0");
	ImGui::Combo("Polygon Mode", m_PolygonMode, "Fill\0Line\0Point\0");
	ImGui::Checkbox("Show Normals", m_DrawNormals);
	ImGui::ColorEdit3("Normals Color", *m_NormalsColor);

	ImGui::End();
}

void UI::Shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
