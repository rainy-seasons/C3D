#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform vec3 color;

void main()
{
	// make it circular via alpha discard
	vec2 center = TexCoord - vec2(0.5);
	float dist = length(center);
	if (dist > 0.5)
		discard;

	FragColor = vec4(color, 1.0);
}