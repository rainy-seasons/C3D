#define WIN32_LEAN_AND_MEAN

#include "Model.h"
#include "ShaderClass.h"

#include <Windows.h>

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 800;

void setClearColor(float red, float green, float blue, float alpha) 
{
	glClearColor(red, green, blue, alpha);
}

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
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE); // Enable face culling
	glCullFace(GL_BACK);    // Cull back faces and keep front faces
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Camera camera(WINDOW_WIDTH, WINDOW_HEIGHT, glm::vec3(0.0f, 3.0f, 0.0f));

	Model model("models/test/statue/scene.gltf");
	Model grass("models/test/grass/scene.gltf");
	Model ground("models/test/ground/scene.gltf");

	int renderMode = 0;

	// For FPS counter
	double prevTime = 0.0;
	double currTime = 0.0;
	double timeDiff;
	unsigned int counter = 0;

	//glfwSwapInterval(0); // Disables vsync if it's not forced by drivers

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

		model.Draw(shaderProgram, camera);
		//ground.Draw(shaderProgram, camera);

		//TODO: Set this up to be automatic
		glDisable(GL_CULL_FACE);
		//glEnable(GL_BLEND);
		grass.Draw(shaderProgram, camera);
		glDepthMask(GL_TRUE); // Re-enable depth writing
		glEnable(GL_CULL_FACE);
		//glDisable(GL_BLEND);

		// Set the background color to grey if renderMode is set to textureless depth
		// Otherwise, make it blue
		if (renderMode == 0)
			setClearColor(0.85f, 0.85f, 0.9f, 1.0f);
		else
			setClearColor(0.07f, 0.13f, 0.17f, 1.0f);


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
		if (GetAsyncKeyState(VK_RIGHT))
		{
			renderMode++;
			if (renderMode > 4)
			{
				renderMode = 0;
			}
			std::cout << "Rendering Mode : " << renderMode << std::endl;
			Sleep(100);
		}

		glUniform1i(glGetUniformLocation(shaderProgram.ID, "renderMode"), renderMode);

		glfwSwapBuffers(window); // Swap back buffer with front buffer
		glfwPollEvents(); // Process GLFW events
	}

	shaderProgram.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}