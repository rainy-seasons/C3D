#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "ShaderClass.h"

class Camera
{
public:
	Camera(int width, int height, glm::vec3 position);

	glm::vec3 Position;
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 cameraMatrix = glm::mat4(1.0f); 

	bool m_bLeftMouseClick;

	int width, height;
	float speed = 0.1f;
	float sensitivity = 100.0f;

	void Matrix(Shader &shader, const char* uniform); // Exports the camera matrix to a shader
	void UpdateMatrix(float FOVdeg, float nearPlane, float farPlane);  // Updates the camera matrix to the vertex shader
	void Inputs(GLFWwindow* window);
};
#endif
