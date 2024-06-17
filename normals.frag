#version 330 core

uniform vec4 normalColor;
out vec4 FragColor;

void main()
{
	FragColor = vec4(normalColor);
}