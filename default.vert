#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTex;

// Outputs the color for the frag shader
out vec3 color;

// Outputs the texture coords to the frag shader
out vec2 texCoord;

// Controls scale of vertices
uniform float scale;

// Matrices needed for 3D perspective view
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	// Outputs the positions and coords of all vertices
	gl_Position = projection * view * model * vec4(aPos, 1.0);

	color = aColor; // Assigns the colors from the vertex data to "color" variable
	texCoord = aTex;
}