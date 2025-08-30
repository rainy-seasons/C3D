#pragma once
#include <string>

#include "glm/glm.hpp"

#include "Camera.h"
#include "Model.h"
#include "Skybox.h"
#include "C3D_UI.h"
#include "ShaderClass.h"
#include "Light.h"

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
	int m_width, m_height;

	bool m_drawUI;

	UI*     m_gui;
	Model*  m_model;
	Model*  m_grass;
	Model*  m_lightSphere;
	Skybox* m_skybox;
	Camera* m_camera;
	Shader* m_mainShader;
	Shader* m_normalShader;

	std::vector<Light> m_lightsVec;

	GLFWwindow* m_window;

	UIParams m_uiParams{
		2,                        // renderMode
		0,                        // polygonMode
		true,                     // drawGrass
		false,                    // drawNormals
		{0.0f, 1.0f, 0.0f, 1.0f}, // normalsColor[]
		0.01f,                    // normalLength
		{0.0f, 0.0f, 0.0f},       // modelRotation[]
		false,					  // showLightPositions
		&m_lightsVec              // pointer to lights vector
	};
};
