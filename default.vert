#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;

out DATA
{
    vec3 Normal;
	vec3 color;
	vec2 texCoord;
    mat4 projection;
} data_out;

// Outputs variables to frag shader
out vec3 currPos;
out vec3 Normal;
out vec3 color;
out vec2 texCoord;

uniform mat4 camMatrix;
uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;


void main()
{
	// CORRECT //
	//gl_Position = model * translation * rotation * scale * vec4(aPos, 1.0f);
	//data_out.Normal = aNormal;
	//data_out.Normal = normalize(mat3(transpose(inverse(model * rotation))) * aNormal); // Correct normal transformation
	//data_out.color = aColor;
	//data_out.texCoord = mat2(0.0, -1.0, 1.0, 0.0) * aTex;
	//data_out.projection = camMatrix;

	//gl_Position = model * translation * rotation * scale * vec4(aPos, 1.0f);
	//gl_Position = camMatrix * vec4(currPos, 1.0);
	//data_out.Normal = aNormal;
	//data_out.color = aColor;
	//data_out.texCoord = mat2(0.0, -1.0, 1.0, 0.0) * aTex;
	//data_out.projection = camMatrix;

	// DEFAULT // 
	currPos = vec3(model * translation * -rotation * scale * vec4(aPos, 1.0f));
	Normal = aNormal;
	color = aColor; // Assigns the colors from the vertex data to "color" variable
	texCoord = mat2(0.0, -1.0, 1.0, 0.0) * aTex;

	gl_Position = camMatrix * vec4(currPos, 1.0); // Outputs the positions and coords of all vertices
}