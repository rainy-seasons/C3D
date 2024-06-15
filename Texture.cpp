#include "Texture.h"

Texture::Texture(const char* image, const char* texType, GLuint slot)
{
	type = texType;
	int imgWidth, imgHeight, numColorChannels;
	stbi_set_flip_vertically_on_load(true); // Flip image because stb reads images opposite of openGL
	unsigned char* bytes = stbi_load(image, &imgWidth, &imgHeight, &numColorChannels, 0);

	// Generate texture
	glGenTextures(1, &ID);
	glActiveTexture(GL_TEXTURE0 + slot); // Assign texture to Texture Unit
	unit = slot;
	glBindTexture(GL_TEXTURE_2D, ID);

	// Configure min/mag algorithm (GL_NEAREST or GL_LINEAR)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Configure the wya the texture repeats (if it does)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if (bytes)
	{
		if (numColorChannels == 4)
		{
			glTexImage2D
			(
				GL_TEXTURE_2D,
				0,
				GL_RGBA,
				imgWidth,
				imgHeight,
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				bytes
			);
		}
		else if (numColorChannels == 3)
		{
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RGBA,
				imgWidth,
				imgHeight,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				bytes
			);
		}
		else if (numColorChannels == 1)
		{
			glTexImage2D
			(
				GL_TEXTURE_2D,
				0,
				GL_RGBA,
				imgWidth,
				imgHeight,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				bytes
			);
		}
		else
		{
			throw std::invalid_argument("Failed to recognize Texture type");
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		throw std::runtime_error("Failed to load texture");
	}


	//glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(bytes);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{
	GLuint tex0Uni = glGetUniformLocation(shader.ID, uniform);
	if (tex0Uni == -1)
	{
		std::cerr << "Warning: Texture uniform " << uniform << " not found in shader: " << std::endl;
	}
	shader.Activate();
	glUniform1i(tex0Uni, unit);
}

void Texture::Bind()
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}