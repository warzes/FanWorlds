#pragma once

#include "RenderResource.h"

class Material
{
public:

	Texture2DRef diffuseTexture = nullptr;

	glm::vec3 ambientColor = glm::vec3{ 1.0f };
	glm::vec3 diffuseColor = glm::vec3{ 1.0f };
	glm::vec3 specularColor = glm::vec3{ 1.0f };
	float shininess = 1.0f;
};

class VertexMesh
{
public:
	bool operator==(const VertexMesh& v) const { return position == v.position && normal == v.normal && color == v.color && texCoord == v.texCoord; }

	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 texCoord;
};

class Mesh
{
public:
	std::vector<VertexMesh> vertices;
	std::vector<uint32_t> indices;
	Material material;

	GeometryBufferRef geometry;

	// bouncing box
	glm::vec3 min;
	glm::vec3 max;
};

class Model
{
public:
	std::vector<Mesh> subMeshes;
	// bouncing box
	glm::vec3 min;
	glm::vec3 max;
};
using ModelRef = std::shared_ptr<Model>;