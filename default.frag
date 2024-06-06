#version 330 core
out vec4 FragColor;

// Import the needed variables from the Vertex Shader
in vec3 color;    
in vec2 texCoord;
in vec3 Normal; 
in vec3 currPos;

// Get variables from main function
uniform sampler2D tex0;
uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

void main()
{
	float ambient = 0.2f;

	// Diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(lightPos - currPos);
	float diffuse = max(dot(normal, lightDir), 0.0f);

	// Specular lighting
	float specularLight = 0.50f;
	vec3 viewDir = normalize(camPos - currPos);
	vec3 reflectionDir = reflect(-lightDir, normal);
	float specAmount = pow(max(dot(viewDir, reflectionDir), 0.0f), 8);
	float specular = specAmount * specularLight;

	// output final color
	FragColor = texture(tex0, texCoord) * lightColor * (diffuse + ambient + specular);
}
