#include "C3D.h"

C3D::C3D(int width, int height, const std::string& title)
	: m_width(width), m_height(height)
{
	m_drawUI = true;
	std::fill(std::begin(m_uiParams.modelRotation), std::end(m_uiParams.modelRotation), 0.0f);
	std::fill(std::begin(m_uiParams.normalsColor), std::end(m_uiParams.normalsColor), 0.0f);
	m_uiParams.normalsColor[1] = 1.0f;
	m_uiParams.normalsColor[3] = 1.0f;

	InitGLSetup();
	InitModels();
	InitShaders();
	InitImGui();
}

C3D::~C3D()
{
	delete m_gui;
	delete m_skybox;
	delete m_camera;
	delete m_model;
	delete m_grass;
	delete m_lightSphere;
	delete m_mainShader;
	delete m_normalShader;
	delete m_window;
}

void C3D::InitGLSetup()
{
	glfwInit();

	// Tell GLFW openGL version -- (3.3) 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Use CORE profile

	//GLFWwindow* window = glfwCreateWindow(width, height, "C3D", NULL, NULL);
	m_window = glfwCreateWindow(m_width, m_height, "C3D", NULL, NULL);
	if (m_window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}

	glfwMakeContextCurrent(m_window);

	// Dumb lambda stuff to avoid making a static helper function due to the "this" pointer changing the function signature since it's a member function
	glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		C3D* app = static_cast<C3D*>(glfwGetWindowUserPointer(window));
		if (app) {
			app->key_callback(window, key, scancode, action, mods);
		}
		});


	gladLoadGL(); // Load GLAD
	glViewport(0, 0, m_width, m_height);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
}

void C3D::InitShaders()
{
	m_mainShader = new Shader("default.vert", "default.frag");
	m_normalShader = new Shader("normals.vert", "normals.frag", "normals.geom");

	Light mainLight;
	mainLight.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	mainLight.Pos = glm::vec3(22.0f, 30.0f, 5.0f);
	mainLight.Model = glm::mat4(1.0f);
	mainLight.Model = glm::translate(mainLight.Model, mainLight.Pos);

	m_lightsVec.push_back(mainLight);

	m_mainShader->Activate();
	glUniform4f(glGetUniformLocation(m_mainShader->ID, "lightColor"), mainLight.Color.x, mainLight.Color.y, mainLight.Color.z, mainLight.Color.w);
	glUniform3f(glGetUniformLocation(m_mainShader->ID, "lightPos"), mainLight.Pos.x, mainLight.Pos.y, mainLight.Pos.z);
}

void C3D::InitModels()
{
	m_skybox = new Skybox();
	m_camera = new Camera(m_width, m_height, glm::vec3(0.0f, 0.0f, 2.0f));

	m_model = new Model("res/models/statue/scene.gltf");
	m_grass = new Model("res/models/grass/scene.gltf");

	m_lightSphere = new Model("res/models/lightsphere/scene.gltf");

	m_skybox->Initialize();
}

void C3D::Run()
{
	while (!glfwWindowShouldClose(m_window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear back buffer and depth buffer

		if (!ImGui::GetIO().WantCaptureMouse && !ImGui::GetIO().WantCaptureKeyboard)
			m_camera->Inputs(m_window); // Handle camera inputs

		m_camera->UpdateMatrix(45.0f, 0.1f, 100.0f); // Update and export camera matrix to vertex shader	

		glm::mat4 modelMatrix = glm::mat4(1.0f); // Define model matrix
		modelMatrix = ApplyRotation(modelMatrix);

		// Set shaderProgram uniforms
		glUniformMatrix4fv(glGetUniformLocation(m_mainShader->ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniform1i(glGetUniformLocation(m_mainShader->ID, "renderMode"), m_uiParams.renderMode);

		for (const auto& light : m_lightsVec)
		{
			glUniform4f(glGetUniformLocation(m_mainShader->ID, "lightColor"), light.Color.x, light.Color.y, light.Color.z, light.Color.w);
			glUniform3f(glGetUniformLocation(m_mainShader->ID, "lightPos"), light.Pos.x, light.Pos.y, light.Pos.z);
		}

		// Visualizes normals of main object
		if (m_uiParams.drawNormals)
		{
			// Activate normal shader and set uniforms
			m_normalShader->Activate();
			glUniformMatrix4fv(glGetUniformLocation(m_normalShader->ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(m_camera->GetViewMatrix()));
			glUniformMatrix4fv(glGetUniformLocation(m_normalShader->ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
			glUniform1f(glGetUniformLocation(m_normalShader->ID, "normLen"), m_uiParams.normalLength);
			glUniform4f(glGetUniformLocation(m_normalShader->ID, "normalColor"), m_uiParams.normalsColor[0], m_uiParams.normalsColor[1], m_uiParams.normalsColor[2], m_uiParams.normalsColor[3]);
			if (m_model)
			{
				m_model->Draw(*m_normalShader, *m_camera);
			}
		}

		CheckStates(); // Check for changes in state variables

		// Render skybox
		glm::mat4 view = glm::mat4(glm::mat3(m_camera->GetViewMatrix())); // Strip translation component from view matrix
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)m_width / m_height, 0.1f, 100.0f);
		m_skybox->Render(view, projection);

		// Render main object
		if (m_model)
		{
			m_model->Draw(*m_mainShader, *m_camera);
		}

		if (m_uiParams.drawGrass)
		{
			glDisable(GL_CULL_FACE);
			m_grass->Draw(*m_mainShader, *m_camera);
			glEnable(GL_CULL_FACE);
		}

		if (m_uiParams.showLightPositions && m_lightSphere)
		{
			//m_mainShader->Activate();
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe
			for (const auto& light : m_lightsVec)
			{
				glm::mat4 model = glm::translate(glm::mat4(1.0f), light.Pos);
				//model = glm::scale(model, glm::vec3(0.2f));
				glUniformMatrix4fv(glGetUniformLocation(m_mainShader->ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(model));

				m_lightSphere->Draw(*m_mainShader, *m_camera);
			}
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		if (m_drawUI)
		{
			m_gui->Render();
		}

		glfwSwapBuffers(m_window); // Swap back buffer with front buffer
		glfwPollEvents(); // Process GLFW events
	}
}

void C3D::InitImGui()
{
	auto fileChosenCallback = [this](const std::string& filePath) { SwapModel(m_model, filePath); };

	m_uiParams.callback = fileChosenCallback;

	m_gui = new UI(m_window, m_uiParams);
	m_gui->Setup();
}

void C3D::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, true);
			break;
		case GLFW_KEY_SPACE:
			m_drawUI = !m_drawUI;
			break;
		}
	}
}

glm::mat4 C3D::ApplyRotation(glm::mat4& modelMatrix)
{
	modelMatrix = glm::rotate(modelMatrix, glm::radians(m_uiParams.modelRotation[0]), glm::vec3(1.0f, 0.0f, 0.0f)); // x-axis
	modelMatrix = glm::rotate(modelMatrix, glm::radians(m_uiParams.modelRotation[1]), glm::vec3(0.0f, 1.0f, 0.0f)); // y-axis
	modelMatrix = glm::rotate(modelMatrix, glm::radians(m_uiParams.modelRotation[2]), glm::vec3(0.0f, 0.0f, 1.0f)); // z-axis

	return modelMatrix;
}

bool C3D::FileExists(const std::string& path)
{
	std::ifstream file(path);
	return file.good();
}

std::string C3D::convertBackslashesToForward(const std::string& path)
{
	std::string result;
	result.reserve(path.size()); // Reserve space to avoid multiple allocations

	for (char c : path)
	{
		if (c == '\\')
		{
			result += '/'; // Replace '\' with '/'
		}
		else
		{
			result += c; // Append other characters unchanged
		}
	}

	return result;
}

void C3D::SwapModel(Model*& model, const std::string& path)
{
	std::string correctedPath = convertBackslashesToForward(path);
	std::cout << "Loading model from path: " << correctedPath << std::endl;
	if (!FileExists(correctedPath))
	{
		std::cerr << "Error: File does not exist at path: " << correctedPath << std::endl;
		return;
	}
	try
	{
		if (model == nullptr)
		{
			model = new Model(correctedPath.c_str()); // Makes a new model object if nullptr
		}
		else
		{
			*model = Model(correctedPath.c_str()); // Reassigns the model pointer
		}
		std::cout << "Model loaded successfully." << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error loading model: " << e.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Unknown error occurred while loading model." << std::endl;
	}
}

void C3D::CheckStates()
{
	switch (m_uiParams.polygonMode)
	{
	case 0:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 1:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 2:
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	default:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Set the default to fill just in case something happens
		break;
	}

	// Set the background color to grey if renderMode is set to textureless depth
	// Otherwise, make it blue
	if (m_uiParams.renderMode == 0)
		glClearColor(0.85f, 0.85f, 0.9f, 1.0f);
	else
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
}
