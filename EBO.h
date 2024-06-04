#ifndef EBO_H
#define EBO_H

#include <glad/glad.h>

class EBO
{
public:
	GLuint ID;
	EBO(GLuint* indices, GLsizeiptr size); // Constructor generates Element Buffer Object and links it to indices

	void Bind();
	void Unbind();
	void Delete();
};
#endif
