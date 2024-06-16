#define WIN32_LEAN_AND_MEAN

#include "Model.h"
#include "Skybox.h"
#include "ShaderClass.h"

#include <Windows.h>

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 800;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void setClearColor(float red, float green, float blue, float alpha) 
{
	glClearColor(red, green, blue, alpha);
}

int renderMode = 0;

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

	Skybox skybox;

	Shader shaderProgram("default.vert", "default.frag");
	Shader normalShader("normals.vert", "normals.frag", "normals.geom");


	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(22.0f, 30.0f, 5.0f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);


	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z); 

	glEnable(GL_DEPTH_TEST); // Enable the depth buffer
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE); // Enable face culling
	glCullFace(GL_BACK);    // Cull back faces and keep front faces
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	Camera camera(WINDOW_WIDTH, WINDOW_HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f));

	// Load models
	Model model("res/models/statue/scene.gltf");
	Model grass("res/models/grass/scene.gltf");
	glm::mat4 modelMatrix = glm::mat4(1.0f); // Define model matrix


	// For FPS counter
	double prevTime = 0.0;
	double currTime = 0.0;
	double timeDiff;
	unsigned int counter = 0;

	//glfwSwapInterval(0); // Disables vsync if it's not forced by drivers

	skybox.Initialize();

	bool drawNormals = false;

	while (!glfwWindowShouldClose(window))
	{
		// FPS counter in window title
		currTime = glfwGetTime();
		timeDiff = currTime - prevTime;
		counter++;
		if (timeDiff >= 1.0 / 30.0)
		{
			std::string FPS = std::to_string((1.0 / timeDiff) * counter);
			std::string ms = std::to_string((timeDiff / counter) * 1000);
			std::string newTitle = "C3D - " + FPS + "FPS / " + ms + "ms";
			glfwSetWindowTitle(window, newTitle.c_str());
			prevTime = currTime;
			counter = 0;
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear back buffer and depth buffer

		camera.Inputs(window); // Handle camera inputs
		camera.UpdateMatrix(45.0f, 0.1f, 100.0f); // Update and export camera matrix to vertex shader
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
		model.Draw(shaderProgram, camera);

		normalShader.Activate();
		glUniformMatrix4fv(glGetUniformLocation(normalShader.ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(normalShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniform1f(glGetUniformLocation(normalShader.ID, "normal_length"), 0.1f);

		// TODO
		/* JUST HERE FOR DEBUG PURPOSES - TEMPORARY */
		if (GetAsyncKeyState(VK_SPACE))
		{
			drawNormals = !drawNormals;
			std::cout << "Draw Normals: " << drawNormals << std::endl;
			Sleep(100);
		}
		if (drawNormals)
		{
			model.Draw(normalShader, camera);
		}

		// Render skybox
		glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // Strip translation
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
		skybox.Render(view, projection);

		glDisable(GL_CULL_FACE);
		grass.Draw(shaderProgram, camera);
		glEnable(GL_CULL_FACE);

		// Set the background color to grey if renderMode is set to textureless depth
		// Otherwise, make it blue
		if (renderMode == 0)
			setClearColor(0.85f, 0.85f, 0.9f, 1.0f);
		else
			setClearColor(0.07f, 0.13f, 0.17f, 1.0f);

		glUniform1i(glGetUniformLocation(shaderProgram.ID, "renderMode"), renderMode);

		glfwSwapBuffers(window); // Swap back buffer with front buffer
		glfwPollEvents(); // Process GLFW events
	}

	shaderProgram.Delete();
	normalShader.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
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
			case GLFW_KEY_DOWN:
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				break;
			case GLFW_KEY_UP:
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				break;
			case GLFW_KEY_LEFT:
				glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
				break;
			case GLFW_KEY_RIGHT:
				renderMode++;
				if (renderMode > 4)
				{
					renderMode = 0;
				}
				std::cout << "Rendering Mode : " << renderMode << std::endl;
				Sleep(100);
				break;

		}
	}
}