#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb/stb_image.h"
#include "ShaderClass.h"
#include "Texture.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Camera.h"

#include <Windows.h>

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 800;

//// Triforce
//GLfloat vertices[] =
//{
//	/*           VERTEX COORDINATES      //      COLORS           */
//	-0.5f, -0.5f * float(sqrt(3)) / 3,     0.0f,  0.8f, 0.3f,  0.02f,  // Lower left corner
//	 0.5f, -0.5f * float(sqrt(3)) / 3,     0.0f,  0.8f, 0.3f,  0.02f,  // Lower right corner
//	 0.0f,  0.5f * float(sqrt(3)) * 2 / 3, 0.0f,  1.0f, 0.6f,  0.32f,  // Upper corner
//	-0.25f, 0.5f * float(sqrt(3)) / 6,     0.0f,  0.9f, 0.45f, 0.17f,  // Inner left
//	 0.25f, 0.5f * float(sqrt(3)) / 6,     0.0f,  0.9f, 0.45f, 0.17f,  // Inner right
//	 0.0f, -0.5f * float(sqrt(3)) / 3,     0.0f,  0.8f, 0.30f, 0.02f   // Inner bottom
//};

// Vertices coordinates
GLfloat vertices[] =
{ /*     COORDINATES     /        COLORS      /   TexCoord  */
	-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
	 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	2.5f, 5.0f
};

// Indices for vertices order
GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 1, 4,
	1, 2, 4,
	2, 3, 4,
	3, 0, 4
};

//GLuint indices[] =
//{
//	0, 3, 5, // Lower left triangle
//	3, 2, 4, // Lower right triangle
//	5, 4, 1  // Upper triangle
//};

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

	// Create shader object using default shaders
	Shader shaderProgram("default.vert", "default.frag");

	// Generate and bind Vertex Array Object
	VAO VAO1;
	VAO1.Bind();

	VBO VBO1(vertices, sizeof(vertices)); // Generate Vertex Buffer Object and link to verts
	EBO EBO1(indices, sizeof(indices));  // Generate Element Buffer Object and link to verts

	// Link VBO attributes (coords and colors) to VAO
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Link VBO to VAO
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Link VBO to VAO

	// Unbind to prevent accidental modification
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();


	// Texture
	Texture popCat("pop_cat.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	popCat.texUnit(shaderProgram, "tex0", 0);

	glEnable(GL_DEPTH_TEST); // Enable the depth buffer

	Camera camera(WINDOW_WIDTH, WINDOW_HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f));


	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f); // Background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear back buffer and depth buffer

		shaderProgram.Activate(); // Give OpenGL the ShaderProgram

		camera.Inputs(window); // Handle camera inputs
		camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix"); // Update and export camera matrix to vertex shader

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

		popCat.Bind();

		VAO1.Bind(); // Bind VAO so OpenGL knows how to use it

		//glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0); // Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(int), GL_UNSIGNED_INT, 0); // Draw primitives, number of indices, datatype of indices, index of indices

		glfwSwapBuffers(window); // Swap back buffer with front buffer

		// Process GLFW events
		glfwPollEvents();
	}

	// Delete the objects
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	popCat.Delete();
	shaderProgram.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}