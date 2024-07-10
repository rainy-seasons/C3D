#pragma once

#include "glm/glm.hpp"

struct Light
{
	glm::vec4 Color;
	glm::vec3 Pos;
	glm::mat4 Model;
};
