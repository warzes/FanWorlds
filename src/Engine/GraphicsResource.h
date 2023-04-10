#pragma once

#include "RenderResource.h"

class Material
{
public:
	Material() = default;
	Material(Material&&) = default;
	Material(const Material&) = delete;
	~Material() = default;
	Material& operator=(Material&&) = default;
	Material& operator=(const Material&) = delete;

	inline bool HasDiffuseMap() const { return diffuseTexture != nullptr; }
	inline bool HasSpecularMap() const { return specularTexture != nullptr; }
	inline bool HasNormalMap() const { return normalTexture != nullptr; }
	inline bool HasEmissiveMap() const { return emissiveTexture != nullptr; }

	Texture2DRef diffuseTexture = nullptr;
	Texture2DRef specularTexture = nullptr;
	Texture2DRef normalTexture = nullptr;
	Texture2DRef emissiveTexture = nullptr;

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
	Mesh() = default;
	Mesh(Mesh&&) = default;
	Mesh(const Mesh&) = delete;
	~Mesh() = default;
	Mesh& operator=(Mesh&&) = default;
	Mesh& operator=(const Mesh&) = delete;

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
	Model() = default;
	Model(Model&&) = default;
	Model(const Model&) = delete;
	~Model() = default;
	Model& operator=(Model&&) = default;
	Model& operator=(const Model&) = delete;

	std::vector<Mesh> subMeshes;
	// bouncing box
	glm::vec3 min;
	glm::vec3 max;
};
using ModelRef = std::shared_ptr<Model>;