#ifndef EBO_H
#define EBO_H

#include <glad/glad.h>
#include <vector>

class EBO
{
public:
	GLuint ID;
	EBO(std::vector<GLuint>& indices); // Constructor generates Element Buffer Object and links it to indices

	void Bind();
	void Unbind();
	void Delete();
};
#endif
