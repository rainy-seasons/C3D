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

//float skyboxVertices[] = {
//	// Positions          
//	-1.0f,  1.0f, -1.0f,
//	-1.0f, -1.0f, -1.0f,
//	 1.0f, -1.0f, -1.0f,
//	 1.0f, -1.0f, -1.0f,
//	 1.0f,  1.0f, -1.0f,
//	-1.0f,  1.0f, -1.0f,
//
//	-1.0f, -1.0f,  1.0f,
//	-1.0f, -1.0f, -1.0f,
//	-1.0f,  1.0f, -1.0f,
//	-1.0f,  1.0f, -1.0f,
//	-1.0f,  1.0f,  1.0f,
//	-1.0f, -1.0f,  1.0f,
//
//	 1.0f, -1.0f, -1.0f,
//	 1.0f, -1.0f,  1.0f,
//	 1.0f,  1.0f,  1.0f,
//	 1.0f,  1.0f,  1.0f,
//	 1.0f,  1.0f, -1.0f,
//	 1.0f, -1.0f, -1.0f,
//
//	-1.0f, -1.0f,  1.0f,
//	-1.0f,  1.0f,  1.0f,
//	 1.0f,  1.0f,  1.0f,
//	 1.0f,  1.0f,  1.0f,
//	 1.0f, -1.0f,  1.0f,
//	-1.0f, -1.0f,  1.0f,
//
//	-1.0f,  1.0f, -1.0f,
//	 1.0f,  1.0f, -1.0f,
//	 1.0f,  1.0f,  1.0f,
//	 1.0f,  1.0f,  1.0f,
//	-1.0f,  1.0f,  1.0f,
//	-1.0f,  1.0f, -1.0f,
//
//	-1.0f, -1.0f, -1.0f,
//	-1.0f, -1.0f,  1.0f,
//	 1.0f, -1.0f, -1.0f,
//	 1.0f, -1.0f, -1.0f,
//	-1.0f, -1.0f,  1.0f,
//	 1.0f, -1.0f,  1.0f
//};
//
//unsigned int skyboxIndices[] = {
//	// Right
//	1, 2, 6,
//	6, 5, 1,
//	// Left
//	0, 4, 7,
//	7, 3, 0,
//	// Top
//	4, 5, 6,
//	6, 7, 4,
//	// Bottom
//	0, 3, 2,
//	2, 1, 0,
//	// Back
//	0, 1, 5,
//	5, 4, 0,
//	// Front
//	3, 7, 6,
//	6, 2, 3
//};

float skyboxVertices[] =
{
	//   Coordinates
	-1.0f, -1.0f,  1.0f,//        7--------6
	 1.0f, -1.0f,  1.0f,//       /|       /|
	 1.0f, -1.0f, -1.0f,//      4--------5 |
	-1.0f, -1.0f, -1.0f,//      | |      | |
	-1.0f,  1.0f,  1.0f,//      | 3------|-2
	 1.0f,  1.0f,  1.0f,//      |/       |/
	 1.0f,  1.0f, -1.0f,//      0--------1
	-1.0f,  1.0f, -1.0f
};

unsigned int skyboxIndices[] =
{
	// Right
	1, 2, 6,
	6, 5, 1,
	// Left
	0, 4, 7,
	7, 3, 0,
	// Top
	4, 5, 6,
	6, 7, 4,
	// Bottom
	0, 3, 2,
	2, 1, 0,
	// Back
	0, 1, 5,
	5, 4, 0,
	// Front
	3, 7, 6,
	6, 2, 3
};



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
	Shader skyboxShader("skybox.vert", "skybox.frag");

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(22.0f, 30.0f, 5.0f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);


	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z); 
	skyboxShader.Activate();
	glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);

	glEnable(GL_DEPTH_TEST); // Enable the depth buffer
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE); // Enable face culling
	glCullFace(GL_BACK);    // Cull back faces and keep front faces
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Camera camera(WINDOW_WIDTH, WINDOW_HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f));

	// Load models
	Model model("models/test/statue/scene.gltf");
	Model grass("models/test/grass/scene.gltf");
	//Model ground("models/test/ground/scene.gltf");

	int renderMode = 0;

	// For FPS counter
	double prevTime = 0.0;
	double currTime = 0.0;
	double timeDiff;
	unsigned int counter = 0;

	//glfwSwapInterval(0); // Disables vsync if it's not forced by drivers

	// Create VAO, VBO, and EBO for the skybox
	unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	std::string facesCubemap[6] =
	{
		"res/cubemaps/default/right.jpg",
		"res/cubemaps/default/left.jpg",
		"res/cubemaps/default/top.jpg",
		"res/cubemaps/default/bottom.jpg",
		"res/cubemaps/default/front.jpg",
		"res/cubemaps/default/back.jpg"
	};

	unsigned int cubemapTex;
	glGenTextures(1, &cubemapTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTex);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// These are necessary for hiding seams
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, numChannels;
		unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &numChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false); // Cubemaps are loaded from top left
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
		}
		stbi_image_free(data);
	}

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

		// Disable these for proper skybox rendering
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_FALSE);
		glDisable(GL_CULL_FACE);

		skyboxShader.Activate();
		glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // Strip the translation component from the view matrix
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// Render skybox
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTex);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Re-enable
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);
		glEnable(GL_CULL_FACE);

		//TODO: Set this up to be automatic
		glDisable(GL_CULL_FACE);
		//glEnable(GL_BLEND);
		grass.Draw(shaderProgram, camera);
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
	skyboxShader.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}