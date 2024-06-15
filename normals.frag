#version 330 core

out vec4 FragColor;

// Define dummy uniforms to avoid errors
uniform sampler2D diffuse0;
uniform sampler2D specular0;

void main()
{
	FragColor = vec4(1.00f, 0.00f, 0.0f, 1.00f);
}