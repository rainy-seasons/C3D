#define WIN32_LEAN_AND_MEAN

#include "Model.h"
#include "ShaderClass.h"

#include <Windows.h>

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 800;

int main()
{
	glfwInit();

	// Tell GLFW openGL version -- (3.3) 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Use CORE profile


	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL Test", NULL, NULL);
	if (window == NULL) 
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	gladLoadGL(); // Load GLAD
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	// Create shader object using default shaders and store mesh data in vectors
	Shader shaderProgram("default.vert", "default.frag");

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(22.0f, 30.0f, 5.0f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z); 

	glEnable(GL_DEPTH_TEST); // Enable the depth buffer
	glDepthFunc(GL_LESS);

	Camera camera(WINDOW_WIDTH, WINDOW_HEIGHT, glm::vec3(0.0f, 3.0f, 0.0f));

	Model model("models/test/sword/scene.gltf");


	while (!glfwWindowShouldClose(window))
	{
	//	glClearColor(0.95f, 0.13f, 0.17f, 1.0f); // Background color
		glClearColor(0.85f, 0.85f, 0.90f, 1.0f); // Background color (grey)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear back buffer and depth buffer

		camera.Inputs(window); // Handle camera inputs
		camera.UpdateMatrix(45.0f, 0.1f, 100.0f); // Update and export camera matrix to vertex shader

		model.Draw(shaderProgram, camera);


		/* JUST HERE FOR TESTING FOR NOW */
		if (GetAsyncKeyState(VK_DOWN))
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		if (GetAsyncKeyState(VK_UP))
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		if (GetAsyncKeyState(VK_LEFT))
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		}

		glfwSwapBuffers(window); // Swap back buffer with front buffer

		// Process GLFW events
		glfwPollEvents();
	}

	// Delete the objects

	shaderProgram.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}