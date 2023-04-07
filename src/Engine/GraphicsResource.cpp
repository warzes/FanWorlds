#include "stdafx.h"
#include "GraphicsResource.h"
#include "GraphicsSystem.h"
#include "RenderSystem.h"
//-----------------------------------------------------------------------------
namespace std
{
	template <>
	struct hash<VertexMesh>
	{
		size_t operator()(const VertexMesh& vertex) const
		{
			return ((hash<glm::vec3>()(vertex.position) ^ (hash<glm::vec2>()(vertex.texCoord) << 1)) >> 1);
		}
	};
} // namespace std
//-----------------------------------------------------------------------------
ModelRef GraphicsSystem::CreateModel(const char* fileName, const char* pathMaterialFiles)
{
	if (std::string(fileName).find(".obj") != std::string::npos)
		return loadObjFile(fileName, pathMaterialFiles);
	return {};
}
//-----------------------------------------------------------------------------
ModelRef GraphicsSystem::CreateModel(std::vector<Mesh>&& meshes)
{
	return createMeshBuffer(std::move(meshes));
}
//-----------------------------------------------------------------------------
void GraphicsSystem::Draw(ModelRef model)
{
	for (size_t i = 0; i < model->subMeshes.size(); i++)
	{
		if (GetRenderSystem().IsValid(model->subMeshes[i].geometry))
		{
			GetRenderSystem().Bind(model->subMeshes[i].material.diffuseTexture, 0);
			GetRenderSystem().Draw(model->subMeshes[i].geometry->vao, PrimitiveTopology::Triangles);
		}
	}
}
//-----------------------------------------------------------------------------
std::vector<glm::vec3> GraphicsSystem::GetVertexInMesh(const Mesh& mesh) const
{
	std::vector<glm::vec3> v;
	v.reserve(mesh.indices.size());
	// востановление треугольников по индексному буферу
	for (size_t i = 0; i < mesh.indices.size(); i++)
		v.push_back(mesh.vertices[mesh.indices[i]].position);
	return v;
}
//-----------------------------------------------------------------------------
std::vector<glm::vec3> GraphicsSystem::GetVertexInModel(ModelRef model) const
{
	std::vector<glm::vec3> v;
	for (size_t i = 0; i < model->subMeshes.size(); i++)
	{
		auto subV = GetVertexInMesh(model->subMeshes[i]);
		// https://www.techiedelight.com/ru/concatenate-two-vectors-cpp/
		std::move(subV.begin(), subV.end(), std::back_inserter(v));
	}
	return v;
}
//-----------------------------------------------------------------------------
ModelRef GraphicsSystem::createMeshBuffer(std::vector<Mesh>&& meshes)
{
	const std::vector<VertexAttribute> formatVertex =
	{
		{.location = 0, .size = 3, .normalized = false, .stride = sizeof(VertexMesh), .offset = (void*)offsetof(VertexMesh, position)},
		{.location = 1, .size = 3, .normalized = false, .stride = sizeof(VertexMesh), .offset = (void*)offsetof(VertexMesh, normal)},
		{.location = 2, .size = 3, .normalized = false, .stride = sizeof(VertexMesh), .offset = (void*)offsetof(VertexMesh, color)},
		{.location = 3, .size = 2, .normalized = false, .stride = sizeof(VertexMesh), .offset = (void*)offsetof(VertexMesh, texCoord)}
	};

	ModelRef model(new Model());
	model->subMeshes = std::move(meshes);
	model->min = model->subMeshes[0].min;
	model->max = model->subMeshes[0].max;

	for (size_t i = 0; i < model->subMeshes.size(); i++)
	{
		model->subMeshes[i].geometry = GetRenderSystem().CreateGeometryBuffer(
			ResourceUsage::Static,
			model->subMeshes[i].vertices.size(), sizeof(model->subMeshes[i].vertices[0]), model->subMeshes[i].vertices.data(),
			model->subMeshes[i].indices.size(), IndexType::Uint32, model->subMeshes[i].indices.data(), 
			formatVertex);

		// compute AABB
		{
			model->min = glm::min(model->min, model->subMeshes[i].min);
			model->max = glm::min(model->max, model->subMeshes[i].max);
		}
	}
	return model;
}
//-----------------------------------------------------------------------------
ModelRef GraphicsSystem::loadObjFile(const char* fileName, const char* pathMaterialFiles)
{
	tinyobj::ObjReaderConfig readerConfig;
	readerConfig.mtl_search_path = pathMaterialFiles; // Path to material files

	tinyobj::ObjReader reader;
	if (!reader.ParseFromFile(fileName, readerConfig))
	{
		if (!reader.Error().empty())
			Error("TinyObjReader: " + reader.Error());
		return {};
	}
	if (!reader.Warning().empty())
		Warning("TinyObjReader: " + reader.Warning());

	auto& attributes = reader.GetAttrib();
	auto& shapes = reader.GetShapes();
	auto& materials = reader.GetMaterials();
	const bool isFindMaterials = !materials.empty();

	std::vector<Mesh> tempMesh(materials.size());
	std::vector<std::unordered_map<VertexMesh, uint32_t>> uniqueVertices(materials.size());
	if (tempMesh.empty())
	{
		tempMesh.resize(1);
		uniqueVertices.resize(1);
	}

	// Loop over shapes
	for (size_t shapeId = 0; shapeId < shapes.size(); shapeId++)
	{
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t faceId = 0; faceId < shapes[shapeId].mesh.num_face_vertices.size(); faceId++)
		{
			const size_t fv = static_cast<size_t>(shapes[shapeId].mesh.num_face_vertices[faceId]);

			// per-face material
			int materialId = shapes[shapeId].mesh.material_ids[faceId];
			if (materialId < 0) materialId = 0;

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++)
			{
				// access to vertex
				const tinyobj::index_t idx = shapes[shapeId].mesh.indices[index_offset + v];

				// vertex position
				const tinyobj::real_t vx = attributes.vertices[3 * size_t(idx.vertex_index) + 0];
				const tinyobj::real_t vy = attributes.vertices[3 * size_t(idx.vertex_index) + 1];
				const tinyobj::real_t vz = attributes.vertices[3 * size_t(idx.vertex_index) + 2];

				// Check if `normal_index` is zero or positive. negative = no normal data
				tinyobj::real_t nx = 0.0f;
				tinyobj::real_t ny = 0.0f;
				tinyobj::real_t nz = 0.0f;
				if (idx.normal_index >= 0)
				{
					nx = attributes.normals[3 * size_t(idx.normal_index) + 0];
					ny = attributes.normals[3 * size_t(idx.normal_index) + 1];
					nz = attributes.normals[3 * size_t(idx.normal_index) + 2];
				}

				// Check if `texcoord_index` is zero or positive. negative = no texcoord data
				tinyobj::real_t tx = 0;
				tinyobj::real_t ty = 0;
				if (idx.texcoord_index >= 0)
				{
					tx = attributes.texcoords[2 * size_t(idx.texcoord_index) + 0];
					ty = attributes.texcoords[2 * size_t(idx.texcoord_index) + 1];
				}

				// vertex colors
				const tinyobj::real_t r = attributes.colors[3 * size_t(idx.vertex_index) + 0];
				const tinyobj::real_t g = attributes.colors[3 * size_t(idx.vertex_index) + 1];
				const tinyobj::real_t b = attributes.colors[3 * size_t(idx.vertex_index) + 2];

				VertexMesh vertex = {
					.position = { vx, vy, vz },
					.normal = { nx, ny, nz },
					.color = { r, g, b },
					.texCoord = { tx,ty }
				};

				if (uniqueVertices[materialId].count(vertex) == 0)
				{
					uniqueVertices[materialId][vertex] = static_cast<uint32_t>(tempMesh[materialId].vertices.size());
					tempMesh[materialId].vertices.emplace_back(vertex);
				}

				tempMesh[materialId].indices.emplace_back(uniqueVertices[materialId][vertex]);
			}
			index_offset += fv;
		}
	}

	// load materials
	if (isFindMaterials)
	{
		for (size_t i = 0; i < materials.size(); i++)
		{
			if (materials[i].diffuse_texname.empty()) continue;

			std::string diffuseMap = pathMaterialFiles + materials[i].diffuse_texname;
			tempMesh[i].material.diffuseTexture = GetRenderSystem().CreateTexture2D(diffuseMap.c_str(), true);
		}
	}

	// compute AABB
	{
		for (size_t i = 0; i < tempMesh.size(); i++)
		{
			tempMesh[0].min = tempMesh[i].vertices[0].position;
			tempMesh[0].max = tempMesh[i].vertices[0].position;
			for (size_t j = 0; j < tempMesh[i].vertices.size(); j++)
			{
				tempMesh[i].min = glm::min(tempMesh[0].min, tempMesh[i].vertices[j].position);
				tempMesh[i].max = glm::max(tempMesh[0].max, tempMesh[i].vertices[j].position);
			}
		}
	}

	return createMeshBuffer(std::move(tempMesh));
}
//-----------------------------------------------------------------------------