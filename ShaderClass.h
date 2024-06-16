#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>

std::string get_file_contents(const char* filename);

class Shader
{
public:
	GLuint ID; // Reference ID of the shader program
	Shader(const char* vertexFile, const char* fragmentFile); 
	Shader(const char* vertexFile, const char* fragmentFile, const char* geometryFile); 
	void Activate();
	void Delete();

private:
	// Checks if the shaders have compiled properly
	void compileErrors(unsigned int shader, const char* type);
};

#endif
