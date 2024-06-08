#include "Model.h"

Model::Model(const char* file)
{
	std::string text = get_file_contents(file);
	JSON = json::parse(text);

	Model::file = file;
	data = getData();

	traverseNode(0);
}

void Model::Draw(Shader& shader, Camera& camera)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Mesh::Draw(shader, camera, matricesMeshes[i]);
	}
}

void Model::loadMesh(unsigned int indMesh)
{
	std::cout << "Loading Mesh: " << indMesh << std::endl;

	if (indMesh >= JSON["meshes"].size())
	{
		std::cerr << "ERROR: Mesh index out of bounds: " << indMesh << std::endl;
	}

	unsigned int posAccInd = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["POSITION"];
	unsigned int normalAccInd = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["NORMAL"];
	unsigned int texAccInd = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["TEXCOORD_0"];
	unsigned int indAccInd = JSON["meshes"][indMesh]["primitives"][0]["indices"];


	std::vector<float> positionVec = getFloats(JSON["accessors"][posAccInd]);
	std::vector<glm::vec3> positions = groupFloatsVec3(positionVec);
	std::cout << "Positions Loaded: " << positions.size() << std::endl;

	std::vector<float> normalVec = getFloats(JSON["accessors"][normalAccInd]);
	std::vector<glm::vec3> normals = groupFloatsVec3(normalVec);
	std::cout << "Normals Loaded: " << normals.size() << std::endl;

	std::vector<float> texUVsVec = getFloats(JSON["accessors"][texAccInd]);
	std::vector<glm::vec2> texUVs = groupFloatsVec2(texUVsVec);
	std::cout << "Texture UVs Loaded: " << texUVs.size() << std::endl;

	std::vector<Vertex> vertices = assembleVertices(positions, normals, texUVs);
	std::cout << "Vertices Assembled: " << vertices.size() << std::endl;

	std::vector<GLuint> indices = getIndices(JSON["accessors"][indAccInd]);
	std::cout << "Indices Loaded: " << indices.size() << std::endl;

	std::vector<Texture> textures = getTextures();
	std::cout << "Textures Loaded: " << textures.size() << std::endl;

	meshes.push_back(Mesh(vertices, indices, textures));
	std::cout << "Mesh successfully assembled." << std::endl;
}

void Model::traverseNode(unsigned int next, glm::mat4 matrix)
{
	if (next >= JSON["nodes"].size()) {
		std::cerr << "Error: Node index out of bounds: " << next << std::endl;
		return;
	}
	json node = JSON["nodes"][next];
	//std::cout << "Traversing node " << next << ": " << node.dump(4) << std::endl;

	glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f); // Get translation if it exists
	if (node.contains("translation"))
	{
		for (unsigned int i = 0; i < node["translation"].size(); i++)
		{
			translation[i] = node["translation"][i];
		}
	}
	glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // Get quaternion if it exists
	if (node.contains("rotation"))
	{
		float rotValues[4] =
		{
			// GLTF encodes quaternions as (x, y, z, w)
			// GLM uses (w, x, y, z)
			node["rotation"][3],
			node["rotation"][0],
			node["rotation"][1],
			node["rotation"][2],
		};
		rotation = glm::make_quat(rotValues);
	}
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f); // Get scale if it exists
	if (node.contains("scale"))
	{
		for (unsigned int i = 0; i < node["scale"].size(); i++)
		{
			scale[i] = node["scale"][i];
		}
	}
	glm::mat4 matNode = glm::mat4(1.0f); // Get matrix if it exists
	if (node.contains("matrix"))
	{
		float matValues[16];
		for (unsigned int i = 0; i < node["matrix"].size(); i++)
		{
			matValues[i] = node["matrix"][i];
		}
		matNode = glm::make_mat4(matValues);
	}

	// Transform initialized matrices
	glm::mat4 transl = glm::translate(glm::mat4(1.0f), translation);
	glm::mat4 rot = glm::mat4_cast(rotation);
	glm::mat4 sca = glm::scale(glm::mat4(1.0f), scale);

	// Multiply all matrices together
	glm::mat4 matNextNode = matrix * matNode * transl * rot * sca;

	if (node.contains("mesh"))
	{
		translationsMeshes.push_back(translation);
		rotationsMeshes.push_back(rotation);
		scalesMeshes.push_back(scale);
		matricesMeshes.push_back(matNextNode);

		loadMesh(node["mesh"]);
	}

	if (node.contains("children"))
	{
		for (unsigned int i = 0; i < node["children"].size(); i++)
			traverseNode(node["children"][i], matNextNode);
	}
}

std::vector<unsigned char> Model::getData()
{
	std::string bytesText; // Stores the raw text 
	std::string uri = JSON["buffers"][0]["uri"]; // Get uri of .bin file

	std::string fileStr = std::string(file);
	std::string fileDir = fileStr.substr(0, fileStr.find_last_of('/') + 1);
	bytesText = get_file_contents((fileDir + uri).c_str());

	std::vector<unsigned char> data(bytesText.begin(), bytesText.end());
	return data;
}

std::vector<float> Model::getFloats(json accessor)
{
	std::vector<float> floats;

	// accessor.value() returns the value of a key if it exists, otherwise returns the 2nd input
	//    useful for getting backup value in case the key doesn't exist
	// bracket notation returns the value of a key - these ones always exist, so there's no need for a backup
	unsigned int buffViewInd = accessor.value("bufferView", 1);   // Buffer view index points to correct buffer view
	unsigned int count = accessor["count"];						  // Count is how many floats to get
	unsigned int accByteOffset = accessor.value("byteOffset", 0); // Offset is the index to the data with respect to element 1 of the buffer view
	std::string type = accessor["type"];						  // How to group the floats -- scalar, vec2, vec3, vec4

	json bufferView = JSON["bufferViews"][buffViewInd];
	unsigned int byteOffset = bufferView["byteOffset"];			  // byteOffset is where the buffer view starts -- Two different offsets because sometimes accessors may share the same buffer view

	// Get the number of floats per vertex by type
	unsigned int numPerVert;
	if      (type == "SCALAR") numPerVert = 1;
	else if (type == "VEC2")   numPerVert = 2;
	else if (type == "VEC3")   numPerVert = 3;
	else if (type == "VEC4")   numPerVert = 4;
	else throw std::invalid_argument("Type is invalid. Valid types = SCALAR, VEC2, VEC3, VEC4");

	// Loop through the data
	unsigned int dataStart = byteOffset + accByteOffset;
	unsigned int dataLength = count * 4 * numPerVert;
	for (unsigned int i = dataStart; i < dataStart + dataLength; i)
	{
		unsigned char bytes[] = { data[i++], data[i++], data[i++], data[i++] };
		float value;
		std::memcpy(&value, bytes, sizeof(float)); // Copy bytes to a float
		floats.push_back(value); // Push the new float to the floats vector
	}

	return floats;
}


std::vector<GLuint> Model::getIndices(json accessor)
{
	std::vector<GLuint> indices;

	//unsigned int buffViewInd = accessor.value("bufferView", 0);
	unsigned int buffViewInd = accessor["bufferView"];
	unsigned int count = accessor["count"];
	unsigned int componentType = accessor["componentType"];

	if (componentType != 5123 && componentType != 5125)
	{
		throw std::runtime_error("UNSUPPORTED COMPONENTTYPE FOR INDICES");
	}

	unsigned int accByteOffset = accessor.value("byteOffset", 0);
	auto& bufferView = JSON["bufferViews"][buffViewInd];
	unsigned int byteOffset = bufferView["byteOffset"];

	unsigned int dataStart = byteOffset + accByteOffset;
	if (componentType == 5125) // UNSIGNED INT
	{
		//for (unsigned int i = dataStart; i < byteOffset + accByteOffset + count * 4; i)
		for (unsigned int i = dataStart; i < dataStart + count * 4; i)
		{
			unsigned char bytes[] = { data[i++], data[i++], data[i++], data[i++] };
			unsigned int value;
			std::memcpy(&value, bytes, sizeof(unsigned int));
			indices.push_back((GLuint)value);
		}
	}
	else if (componentType == 5123) // UNSIGNED SHORT
	{
		//for (unsigned int i = dataStart; i < byteOffset + accByteOffset + count * 2; i)
		for (unsigned int i = dataStart; i < dataStart + count * 2; i)
		{
			unsigned char bytes[] = { data[i++], data[i++] };
			unsigned short value;
			std::memcpy(&value, bytes, sizeof(unsigned short));
			indices.push_back((GLuint)value);
		}
	}
	else if (componentType == 5122) // SHORT
	{
		//for (unsigned int i = dataStart; i < byteOffset + accByteOffset + count * 2; i)
		for (unsigned int i = dataStart; i < dataStart + count * 2; i)
		{
			unsigned char bytes[] = { data[i++], data[i++] };
			short value;
			std::memcpy(&value, bytes, sizeof(short));
			indices.push_back((GLuint)value);
		}
	}
	else
	{
		std::cerr << "UNSUPPORTED componentType: " << componentType << std::endl;
	}
	return indices;
}

std::vector<Texture> Model::getTextures()
{
	std::vector<Texture> textures;

	std::string fileStr = std::string(file);
	std::string fileDir = fileStr.substr(0, fileStr.find_last_of('/') + 1);
	

	/* TODO: This method is not ideal. Should be improved later. */
	for (unsigned int i = 0; i < JSON["images"].size(); i++)
	{
		// uri of current texture
		std::string texturePath = JSON["images"][i]["uri"];

		// Check if a texture is already loaded and skip it if so
		// Prevents double loading textures
		bool skip = false;
		for (unsigned int j = 0; j < loadedTexName.size(); j++)
		{
			if (loadedTexName[j] == texturePath)
			{
				textures.push_back(loadedTex[j]);
				skip = true;
				break;
			}
		}

		if (!skip)
		{
			std::string fullPath = fileDir + texturePath;

			if (texturePath.find("baseColor") != std::string::npos) // Check if it contains 'baseColor' -- this means it's a diffuse/albedo
			{
				Texture diffuse = Texture(fullPath.c_str(), "diffuse", loadedTex.size());
				textures.push_back(diffuse);
				loadedTex.push_back(diffuse);
				loadedTexName.push_back(texturePath);
			}
			else if (texturePath.find("metallicRoughness") != std::string::npos) // If it contains 'metallicRoughness' then it can kind of be used as a specular map
			{
				Texture specular = Texture(fullPath.c_str(), "specular", loadedTex.size());
				textures.push_back(specular);
				loadedTex.push_back(specular);
				loadedTexName.push_back(texturePath);
			}
		}
	}
	return textures;
}

std::vector<Vertex> Model::assembleVertices(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<glm::vec2> texUVs)
{
	std::vector<Vertex> vertices;
	for (int i = 0; i < positions.size(); i++)
	{
		vertices.push_back(
			Vertex
			{ 
				positions[i], 
				normals[i], 
				glm::vec3(1.0f, 1.0f, 1.0f), 
				texUVs[i]
			});
	}
	return vertices;
}

std::vector<glm::vec2> Model::groupFloatsVec2(std::vector<float> floats)
{
	std::vector<glm::vec2> vectors;
	for (int i = 0; i < floats.size(); i)
	{
		vectors.push_back(glm::vec2(floats[i++], floats[i++]));
	}
	return vectors;
}

std::vector<glm::vec3> Model::groupFloatsVec3(std::vector<float> floats)
{
	std::vector<glm::vec3> vectors;
	for (int i = 0; i < floats.size(); i)
	{
		vectors.push_back(glm::vec3(floats[i++], floats[i++], floats[i++]));
	}
	return vectors;
}

std::vector<glm::vec4> Model::groupFloatsVec4(std::vector<float> floats)
{
	std::vector<glm::vec4> vectors;
	for (int i = 0; i < floats.size(); i)
	{
		vectors.push_back(glm::vec4(floats[i++], floats[i++], floats[i++], floats[i++]));
	}
	return vectors;
}
