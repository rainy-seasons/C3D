#pragma once
#include <string>

#include "glm/glm.hpp"

#include "Camera.h"
#include "Model.h"
#include "Skybox.h"
#include "C3D_UI.h"
#include "ShaderClass.h"

class C3D 
{
public:
	C3D(int width, int height, const std::string& title);
	~C3D();
	void InitGLSetup();
	void InitImGui();
	void Run();

private:
	void InitShaders();
	void InitModels();
	void CheckStates();
	void SwapModel(Model*& model, const std::string& path);
	glm::mat4 ApplyRotation(glm::mat4& modelMatrix);

	bool FileExists(const std::string& path);
	std::string convertBackslashesToForward(const std::string& path);

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
	int   m_renderMode;
	int   m_polygonMode;
	bool  m_drawUI;
	bool  m_drawGrass;
	bool  m_drawNormals;
	float m_normalLength;
	float m_modelRotation[3];
	float m_normalsColor[4];

	int m_width, m_height;

	UI*     m_gui;
	Model*  m_model;
	Model*  m_grass;
	Skybox* m_skybox;
	Camera* m_camera;
	Shader* m_mainShader;
	Shader* m_normalShader;

	GLFWwindow* m_window;
};
