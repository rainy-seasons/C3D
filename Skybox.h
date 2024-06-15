#ifndef SKYBOX_H
#define SKYBOX_H

#include "ShaderClass.h"
#include "glm/glm.hpp"
#include <vector>
#include <string>

class Skybox
{
public:
	Skybox();
	~Skybox();
	void Initialize();
	void Render(const glm::mat4& view, const glm::mat4& projection);
	void LoadCubemap(const std::vector <std::string>& faces);

private:
	unsigned int VAO, VBO, EBO;
	unsigned int cubemapTex;
	Shader skyboxShader;
	std::vector<std::string> facesCubemap;
};

#endif 

