#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h> 
#include "ShaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"


constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 800;

// Equilateral triangle
GLfloat vertices[] =
{
	/*           VERTEX COORDINATES      //      COLORS           */
	-0.5f, -0.5f * float(sqrt(3)) / 3,     0.0f,  0.8f, 0.3f,  0.02f,  // Lower left corner
	 0.5f, -0.5f * float(sqrt(3)) / 3,     0.0f,  0.8f, 0.3f,  0.02f,  // Lower right corner
	 0.0f,  0.5f * float(sqrt(3)) * 2 / 3, 0.0f,  1.0f, 0.6f,  0.32f,  // Upper corner
	-0.25f, 0.5f * float(sqrt(3)) / 6,     0.0f,  0.9f, 0.45f, 0.17f,  // Inner left
	 0.25f, 0.5f * float(sqrt(3)) / 6,     0.0f,  0.9f, 0.45f, 0.17f,  // Inner right
	 0.0f, -0.5f * float(sqrt(3)) / 3,     0.0f,  0.8f, 0.30f, 0.02f   // Inner bottom
};

GLuint indices[] =
{
	0, 3, 5, // Lower left triangle
	3, 2, 4, // Lower right triangle
	5, 4, 1  // Upper triangle
};

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
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float))); // Link VBO to VAO

	// Unbind to prevent accidental modification
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	// Gets ID of uniform "scale"
	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f); // Background color
		glClear(GL_COLOR_BUFFER_BIT); // Clear back buffer and assign color

		shaderProgram.Activate(); // Give OpenGL the ShaderProgram

		glUniform1f(uniID, 0.5f); // Assign a value to the uniform (must be after shaderProgram.activate())

		VAO1.Bind(); // Bind VAO so OpenGL knows how to use it

		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0); // Draw primitives, number of indices, datatype of indices, index of indices

		glfwSwapBuffers(window); // Swap back buffer with front buffer

		// Process GLFW events
		glfwPollEvents();
	}

	// Delete the objects
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	shaderProgram.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}