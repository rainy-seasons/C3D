#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTex;
layout (location = 3) in vec3 aNormal;

// Outputs variables to frag shader
out vec3 color;
out vec2 texCoord;
out vec3 Normal;
out vec3 currPos;

// Imports from main function
uniform mat4 camMatrix;
uniform mat4 model;

void main()
{
	currPos = vec3(model * vec4(aPos, 1.0f));
	gl_Position = camMatrix * vec4(currPos, 1.0); // Outputs the positions and coords of all vertices

	color = aColor; // Assigns the colors from the vertex data to "color" variable
	texCoord = aTex;
	Normal = aNormal;
}