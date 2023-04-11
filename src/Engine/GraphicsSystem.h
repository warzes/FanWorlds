#pragma once

#include "BaseClass.h"
#include "GraphicsResource.h"

class GraphicsSystem : public BaseClass
{
	friend class EngineDevice;
public:
	void Init();
	void Close();

	ModelRef CreateModel(const char* fileName, const char* pathMaterialFiles = "./");
	ModelRef CreateModel(std::vector<Mesh>&& meshes);

	void Draw(Mesh& subMesh);
	void Draw(ModelRef model);
	std::vector<glm::vec3> GetVertexInMesh(const Mesh& mesh) const;
	std::vector<glm::vec3> GetVertexInModel(ModelRef model) const;

private:
	GraphicsSystem() = default;
	GraphicsSystem(GraphicsSystem&&) = delete;
	GraphicsSystem(const GraphicsSystem&) = delete;
	GraphicsSystem& operator=(GraphicsSystem&&) = delete;
	GraphicsSystem& operator=(const GraphicsSystem&) = delete;

	ModelRef createMeshBuffer(std::vector<Mesh>&& meshes);
	ModelRef loadObjFile(const char* fileName, const char* pathMaterialFiles = "./");
	ModelRef loadFbxFile(const char* fileName, const char* pathMaterialFiles = "./");
	void computeSubMeshesAABB(std::vector<Mesh>& meshes);
};