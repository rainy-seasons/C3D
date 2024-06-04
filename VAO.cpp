#include "VAO.h"

// Generate VAO ID
VAO::VAO()
{
	glGenVertexArrays(1, &ID);
}

// Links a VBO attribute to the VAO
void VAO::LinkAttrib(VBO VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
{
	VBO.Bind();
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset); // Configure Vertex Attribute so OpenGL knows how to read VBO
	glEnableVertexAttribArray(layout); // Enable Vertex Attribute so OpenGL knows how to use it
	VBO.Unbind();
}

void VAO::Bind()
{
	glBindVertexArray(ID); // Bind VAO to make it current Vertex Array Object
}

void VAO::Unbind()
{
	glBindVertexArray(0);
}

void VAO::Delete()
{
	glDeleteVertexArrays(1, &ID);
}