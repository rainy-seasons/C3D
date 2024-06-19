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

out vec3 currPos;

uniform mat4 camMatrix;
uniform mat4 modelMatrix;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;


void main()
{
	mat4 modelMat = modelMatrix * translation * rotation * scale;
	mat3 normalMatrix = transpose(inverse(mat3(modelMat)));

	currPos = vec3(modelMat * vec4(aPos, 1.0f));

    data_out.Normal = normalize(normalMatrix * aNormal);
    data_out.texCoord = mat2(0.0, -1.0, 1.0, 0.0) * aTex;
    data_out.projection = camMatrix;

	gl_Position = vec4(currPos, 1.0);
}