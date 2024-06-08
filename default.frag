#version 330 core

out vec4 FragColor;

// Import the needed variables from the Vertex Shader
in vec3 currPos;
in vec3 Normal; 
in vec3 color;    
in vec2 texCoord;

// Get variables from main function
uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

// Quadratic light equation for CG instead of 1/d^2
// We use: i = 1/a*d^2+b*d+1
vec4 PointLight()
{
	vec3 lightVec = lightPos - currPos;
	float dist = length(lightVec);
	//float a = 1.0;
	//float b = 0.4;
	float a = 3;
	float b = 0.8;
	float intensity = 1.0f / (a * dist * dist + b * dist + 1.0f);
	
	float ambient = 0.20f;

	// Diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(lightVec);
	float diffuse = max(dot(normal, lightDir), 0.0f);

	// Specular lighting
	float specularLight = 0.50f;
	vec3 viewDir = normalize(camPos - currPos);
	vec3 reflectionDir = reflect(-lightDir, normal);
	float specAmount = pow(max(dot(viewDir, reflectionDir), 0.0f), 16);
	float specular = specAmount * specularLight;
	return (texture(diffuse0, texCoord) * (diffuse * intensity + ambient) + texture(specular0, texCoord).r * specular * intensity) * lightColor;
}

vec4 DirectLight()
{
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(vec3(1.0f, 1.0f, 0.0f));
	float diffuse = max(dot(normal, lightDir), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDir = normalize(camPos - currPos);
	vec3 reflectionDir = reflect(-lightDir, normal);
	float specAmount = pow(max(dot(viewDir, reflectionDir), 0.0f), 16);
	float specular = specAmount * specularLight;

	return (texture(diffuse0, texCoord) * (diffuse + ambient) + texture(specular0, texCoord).r * specular) * lightColor;
}

vec4 SpotLight()
{
	// controls how big the area that is lit up is
	// THESE ARE ALREADY CALCULATED FROM cos(angle) 
	// So it's faster and saves on cycles
	float outerCone = 0.90f;
	float innerCone = 0.95f;

	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(lightPos - currPos);
	float diffuse = max(dot(normal, lightDir), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDir = normalize(camPos - currPos);
	vec3 reflectionDir = reflect(-lightDir, normal);
	float specAmount = pow(max(dot(viewDir, reflectionDir), 0.0f), 16);
	float specular = specAmount * specularLight;

	// calculates the intensity of the currPos based on its angle to the center of the light cone
	float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDir);
	float intensity = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);

	return (texture(diffuse0, texCoord) * (diffuse * intensity + ambient) + texture(specular0, texCoord).r * specular * intensity) * lightColor;
}

void main()
{
	//FragColor = PointLight();
	//FragColor = SpotLight();
	FragColor = DirectLight();
}
