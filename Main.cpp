#define WIN32_LEAN_AND_MEAN
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Model.h"
#include "Skybox.h"
#include "ShaderClass.h"
#include "C3D_UI.h"

#include <Windows.h>


constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 800;

void CheckStates();
void SwapModel(Model*& model, const std::string& path);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

bool FileExists(const std::string& path);

glm::mat4 ApplyRotation(glm::mat4& modelMatrix);
std::string convertBackslashesToForward(const std::string& path);

void SetClearColor(float red, float green, float blue, float alpha) 
{
	glClearColor(red, green, blue, alpha);
}

// TODO: fix whatever is going on here
int renderMode = 0;
int polygonMode = 0;
bool drawUI = true;
bool drawGrass = true;
bool drawNormals = false;
float modelRotation[3] = { 0.0f, 0.0f, 0.0f };
float normalsColor[] = { 0.0f, 1.0f, 0.0f, 1.0f };
float normalLength = 0.01f;

int main()
{
	glfwInit();

	// Tell GLFW openGL version -- (3.3) 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Use CORE profile

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "C3D", NULL, NULL);
	if (window == NULL) 
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	gladLoadGL(); // Load GLAD
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	Skybox skybox;
	Camera camera(WINDOW_WIDTH, WINDOW_HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f));
	Shader mainShader("default.vert", "default.frag");
	Shader normalShader("normals.vert", "normals.frag", "normals.geom");
	Model* model = new Model("res/models/statue/scene.gltf");
	Model grass("res/models/grass/scene.gltf");

	// Setup GUI
	auto fileChosenCallback = [&model](const std::string& filePath) { SwapModel(model, filePath); };
	UI GUI(window, renderMode, polygonMode, drawGrass, drawNormals, normalsColor, normalLength, modelRotation, fileChosenCallback);
	GUI.Setup();

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(22.0f, 30.0f, 5.0f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	mainShader.Activate();
	glUniform4f(glGetUniformLocation(mainShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(mainShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z); 

	// For FPS counter
	double prevTime = glfwGetTime();
	double currTime = 0.0;
	double timeDiff;
	unsigned int counter = 0;

	//glfwSwapInterval(0); // Disables vsync if it's not forced by drivers (not reccommended)

	skybox.Initialize();

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear back buffer and depth buffer

		// FPS counter in window title
		currTime = glfwGetTime();
		timeDiff = currTime - prevTime;
		counter++;
		if (timeDiff >= 1.0 / 60.0)
		{
			std::string FPS = std::to_string((1.0 / timeDiff) * counter);
			std::string ms = std::to_string((timeDiff / counter) * 1000);
			std::string newTitle = "C3D - " + FPS + "FPS / " + ms + "ms";
			glfwSetWindowTitle(window, newTitle.c_str());
			prevTime = currTime;
			counter = 0;
		}

		if (!ImGui::GetIO().WantCaptureMouse && !ImGui::GetIO().WantCaptureKeyboard)
			camera.Inputs(window); // Handle camera inputs

		camera.UpdateMatrix(45.0f, 0.1f, 100.0f); // Update and export camera matrix to vertex shader

		glm::mat4 modelMatrix = glm::mat4(1.0f); // Define model matrix
		modelMatrix = ApplyRotation(modelMatrix);
		//modelMatrix = glm::rotate(modelMatrix, glm::radians(modelRotation), glm::vec3(0.0f, 1.0f, 0.0f));

		// Set shaderProgram uniforms
		glUniformMatrix4fv(glGetUniformLocation(mainShader.ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniform1i(glGetUniformLocation(mainShader.ID, "renderMode"), renderMode);

		// Visualizes normals of main object
		if (drawNormals)
		{
			// Activate normal shader and set uniforms
			normalShader.Activate();
			glUniformMatrix4fv(glGetUniformLocation(normalShader.ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
			glUniformMatrix4fv(glGetUniformLocation(normalShader.ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
			glUniform1f(glGetUniformLocation(normalShader.ID, "normLen"), normalLength);
			glUniform4f(glGetUniformLocation(normalShader.ID, "normalColor"), normalsColor[0], normalsColor[1], normalsColor[2], normalsColor[3]);
			if (model)
			{
				model->Draw(normalShader, camera);
			}
		}

		CheckStates(); // Check for changes in state variables

		// Render skybox
		glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // Strip translation component from view matrix
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
		skybox.Render(view, projection);

		// Render main object
		if (model)
		{
			model->Draw(mainShader, camera);
		}

		if (drawGrass)
		{
			glDisable(GL_CULL_FACE);
			grass.Draw(mainShader, camera);
			glEnable(GL_CULL_FACE);
		}

		if (drawUI)
		{
			GUI.Render();
		}

		glfwSwapBuffers(window); // Swap back buffer with front buffer
		glfwPollEvents(); // Process GLFW events
	}

	GUI.Shutdown();
	delete(model);
	mainShader.Delete();
	normalShader.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

glm::mat4 ApplyRotation(glm::mat4& modelMatrix)
{
	modelMatrix = glm::rotate(modelMatrix, glm::radians(modelRotation[0]), glm::vec3(1.0f, 0.0f, 0.0f)); // x-axis
	modelMatrix = glm::rotate(modelMatrix, glm::radians(modelRotation[1]), glm::vec3(0.0f, 1.0f, 0.0f)); // y-axis
	modelMatrix = glm::rotate(modelMatrix, glm::radians(modelRotation[2]), glm::vec3(0.0f, 0.0f, 1.0f)); // z-axis

	return modelMatrix;
}

bool FileExists(const std::string& path) 
{
	std::ifstream file(path);
	return file.good();
}

std::string convertBackslashesToForward(const std::string& path) 
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

void SwapModel(Model*& model, const std::string& path)
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
			model = new Model(correctedPath.c_str());
		}
		else
		{
			*model = Model(correctedPath.c_str());
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

void CheckStates()
{
	switch (polygonMode)
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
			break;
	}

	// Set the background color to grey if renderMode is set to textureless depth
	// Otherwise, make it blue
	if (renderMode == 0)
		SetClearColor(0.85f, 0.85f, 0.9f, 1.0f);
	else
		SetClearColor(0.07f, 0.13f, 0.17f, 1.0f);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(window, true);
				break;
			case GLFW_KEY_SPACE:
				drawUI = !drawUI;
				break;
		}
	}
}