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

GLfloat vertices[] =
{ /* VERTEX COORDINATES        COLORS				TEX-COORD			NORMALS	  */
	-1.0f, 0.0f,  1.0f,		0.0f, 0.0f, 0.0f,		0.0f, 0.0f,		0.0f, 1.0f, 0.0f,
	-1.0f, 0.0f, -1.0f,		0.0f, 0.0f, 0.0f,		0.0f, 1.0f,		0.0f, 1.0f, 0.0f,
	 1.0f, 0.0f, -1.0f,		0.0f, 0.0f, 0.0f,		1.0f, 1.0f,		0.0f, 1.0f, 0.0f,
	 1.0f, 0.0f,  1.0f,		0.0f, 0.0f, 0.0f,		1.0f, 0.0f,		0.0f, 1.0f, 0.0f
};

GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3
};

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

// Pyramid
//GLfloat vertices[] =
//{ /*     COORDINATES     /        COLORS               TexCoord            NORMALS       */
//	-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f, 	 0.0f, 0.0f,      0.0f, -1.0f, 0.0f, // Bottom side
//	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 5.0f,      0.0f, -1.0f, 0.0f, // Bottom side
//	 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 5.0f,      0.0f, -1.0f, 0.0f, // Bottom side
//	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.0f, -1.0f, 0.0f, // Bottom side
//
//	-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f, 	 0.0f, 0.0f,     -0.8f, 0.5f,  0.0f, // Left Side
//	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,     -0.8f, 0.5f,  0.0f, // Left Side
//	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,     -0.8f, 0.5f,  0.0f, // Left Side
//
//	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.0f, 0.5f, -0.8f, // Non-facing side
//	 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 0.0f,      0.0f, 0.5f, -0.8f, // Non-facing side
//	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,      0.0f, 0.5f, -0.8f, // Non-facing side
//
//	 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 0.0f,      0.8f, 0.5f,  0.0f, // Right side
//	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.8f, 0.5f,  0.0f, // Right side
//	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,      0.8f, 0.5f,  0.0f, // Right side
//
//	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.0f, 0.5f,  0.8f, // Facing side
//	-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f, 	 0.0f, 0.0f,      0.0f, 0.5f,  0.8f, // Facing side
//	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,      0.0f, 0.5f,  0.8f  // Facing side
//};

// Pyramid
//GLuint indices[] =
//{
//	0, 1, 2,    // Bottom
//	0, 2, 3,    // Bottom
//	4, 6, 5,    // Left
//	7, 9, 8,    // Back
//	10, 12, 11, // Right
//	13, 15, 14  // Front
//};

GLfloat lightVertices[] =
{
	-0.1f, -0.1f,  0.1f,
	-0.1f, -0.1f, -0.1f,
	 0.1f, -0.1f, -0.1f,
	 0.1f, -0.1f,  0.1f,
	-0.1f,  0.1f,  0.1f,
	-0.1f,  0.1f, -0.1f,
	 0.1f,  0.1f, -0.1f,
	 0.1f,  0.1f,  0.1f
};

GLuint lightIndices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 4, 7,
	0, 7, 3,
	3, 7, 6,
	3, 6, 2,
	2, 6, 5,
	2, 5, 1,
	1, 5, 4,
	1, 4, 0,
	4, 5, 6,
	4, 6, 7
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
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float))); // Link VBO to VAO
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float))); // Link VBO to VAO
	VAO1.LinkAttrib(VBO1, 3, 3, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float))); // Link VBO to VAO

	// Unbind to prevent accidental modification
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	Shader lightShader("light.vert", "light.frag"); // Shader for light cube
	VAO lightVAO; // Generate Vertex Array Object
	lightVAO.Bind(); 

	VBO lightVBO(lightVertices, sizeof(lightVertices));
	EBO lightEBO(lightIndices, sizeof(lightIndices)); // Generate EBO and link to indices for light cube

	lightVAO.LinkAttrib(lightVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

	lightVAO.Unbind();
	lightVBO.Unbind();
	lightEBO.Unbind();

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	glm::vec3 pyramidPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat4 pyramidModel = glm::mat4(1.0f);
	pyramidModel = glm::translate(pyramidModel, pyramidPos);

	lightShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
	glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	shaderProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(pyramidModel));
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);


	// Texture
	Texture popCat("pop_cat.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	popCat.texUnit(shaderProgram, "tex0", 0);

	glEnable(GL_DEPTH_TEST); // Enable the depth buffer

	Camera camera(WINDOW_WIDTH, WINDOW_HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f));


	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f); // Background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear back buffer and depth buffer

		camera.Inputs(window); // Handle camera inputs
		camera.UpdateMatrix(45.0f, 0.1f, 100.0f); // Update and export camera matrix to vertex shader

		shaderProgram.Activate(); // Give OpenGL the ShaderProgram

		// Export the camera position to the frag shader for specular lighting
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		camera.Matrix(shaderProgram, "camMatrix"); // Export the camMatrix to the vert shader of the pyramid

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

		glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(int), GL_UNSIGNED_INT, 0); // Draw primitives, number of indices, datatype of indices, index of indices

		lightShader.Activate();
		camera.Matrix(lightShader, "camMatrix");
		lightVAO.Bind();
		glDrawElements(GL_TRIANGLES, sizeof(lightIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

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
	lightVAO.Delete();
	lightVBO.Delete();
	lightEBO.Delete();
	lightShader.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}