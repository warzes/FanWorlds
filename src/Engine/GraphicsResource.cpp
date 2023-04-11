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
	// TODO: переделать сделав нормальное получение расшиерия
	if (std::string(fileName).find(".obj") != std::string::npos)
		return loadObjFile(fileName, pathMaterialFiles);
	else if( std::string(fileName).find(".fbx") != std::string::npos )
		return loadFbxFile(fileName, pathMaterialFiles);

	return {};
}
//-----------------------------------------------------------------------------
ModelRef GraphicsSystem::CreateModel(std::vector<Mesh>&& meshes)
{
	return createMeshBuffer(std::move(meshes));
}
//-----------------------------------------------------------------------------
void GraphicsSystem::Draw(Mesh& subMesh)
{
	if( GetRenderSystem().IsValid(subMesh.geometry) )
	{
		GetRenderSystem().Bind(subMesh.material.diffuseTexture, 0);
		GetRenderSystem().Draw(subMesh.geometry->vao, PrimitiveTopology::Triangles);
	}
}
//-----------------------------------------------------------------------------
void GraphicsSystem::Draw(ModelRef model)
{
	for (size_t i = 0; i < model->subMeshes.size(); i++)
	{
		Draw(model->subMeshes[i]);
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
	model->aabb.min = model->subMeshes[0].globalAABB.min;
	model->aabb.max = model->subMeshes[0].globalAABB.max;

	for (size_t i = 0; i < model->subMeshes.size(); i++)
	{
		model->subMeshes[i].geometry = GetRenderSystem().CreateGeometryBuffer(
			BufferUsage::StaticDraw,
			model->subMeshes[i].vertices.size(), sizeof(model->subMeshes[i].vertices[0]), model->subMeshes[i].vertices.data(),
			model->subMeshes[i].indices.size(), IndexType::Uint32, model->subMeshes[i].indices.data(), 
			formatVertex);

		// compute AABB
		{
			model->aabb.Include(model->subMeshes[i].globalAABB);
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

	std::vector<Mesh> meshes(shapes.size());
	std::vector<int> materialIds(shapes.size());

	// Loop over shapes
	for (size_t shapeId = 0; shapeId < shapes.size(); shapeId++)
	{
		std::unordered_map<VertexMesh, uint32_t> uniqueVertices;

		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t faceId = 0; faceId < shapes[shapeId].mesh.num_face_vertices.size(); faceId++)
		{
			const size_t fv = static_cast<size_t>(shapes[shapeId].mesh.num_face_vertices[faceId]);

			// per-face material
			materialIds[shapeId] = shapes[shapeId].mesh.material_ids[faceId];
			if ( materialIds[shapeId] < 0) materialIds[shapeId] = 0;

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

				if (uniqueVertices.count(vertex) == 0)
				{
					uniqueVertices[vertex] = static_cast<uint32_t>(meshes[shapeId].vertices.size());
					meshes[shapeId].vertices.emplace_back(vertex);
				}

				meshes[shapeId].indices.emplace_back(uniqueVertices[vertex]);
			}
			index_offset += fv;
		}
	}

	// load materials
	if (isFindMaterials)
	{
		for (size_t i = 0; i < shapes.size(); i++)
		{
			if (materials[materialIds[i]].diffuse_texname.empty()) continue;

			std::string diffuseMap = pathMaterialFiles + materials[i].diffuse_texname;
			meshes[i].material.diffuseTexture = GetRenderSystem().CreateTexture2D(diffuseMap.c_str(), true);
		}
	}

	// compute AABB
	computeSubMeshesAABB(meshes);

	return createMeshBuffer(std::move(meshes));
}
//-----------------------------------------------------------------------------
ModelRef GraphicsSystem::loadFbxFile(const char* fileName, const char* pathMaterialFiles)
{
	FILE* file = nullptr;
	errno_t error = fopen_s(&file, fileName, "rb");

	if( !file )
	{
		Error("File FBX: " + std::string(fileName) + " not load!");
		return {};
	}

	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	auto* content = new ofbx::u8[file_size];
	fread(content, 1, file_size, file);

	// Ignoring certain nodes will only stop them from being processed not tokenised (i.e. they will still be in the tree)
	ofbx::LoadFlags flags =
		ofbx::LoadFlags::TRIANGULATE |
		//ofbx::LoadFlags::IGNORE_MODELS |
		ofbx::LoadFlags::IGNORE_BLEND_SHAPES |
		ofbx::LoadFlags::IGNORE_CAMERAS |
		ofbx::LoadFlags::IGNORE_LIGHTS |
		//ofbx::LoadFlags::IGNORE_TEXTURES |
		ofbx::LoadFlags::IGNORE_SKIN |
		ofbx::LoadFlags::IGNORE_BONES |
		ofbx::LoadFlags::IGNORE_PIVOTS |
		//ofbx::LoadFlags::IGNORE_MATERIALS |
		ofbx::LoadFlags::IGNORE_POSES |
		ofbx::LoadFlags::IGNORE_VIDEOS |
		ofbx::LoadFlags::IGNORE_LIMBS |
		//ofbx::LoadFlags::IGNORE_MESHES |
		ofbx::LoadFlags::IGNORE_ANIMATIONS;

	ofbx::IScene* scene = ofbx::load((ofbx::u8*)content, file_size, (ofbx::u16)flags);
	if( !scene )
	{
		Error("Failed to import \"" + std::string(fileName) + ": " + ofbx::getError() + "\n"
			"Please try to convert the FBX file with Autodesk FBX Converter or some other software to the latest version.");
		return {};
	}

	std::vector<Mesh> Meshes;

	int obj_idx = 0;
	int indices_offset = 0;
	int normals_offset = 0;
	const int mesh_count = scene->getMeshCount();
	for( int mesh_idx = 0; mesh_idx < mesh_count; ++mesh_idx ) // по мешам
	{
		const ofbx::Mesh& fbx_mesh = *scene->getMesh(mesh_idx);

		const ofbx::Geometry& geom = *fbx_mesh.getGeometry();

		const ofbx::Vec3* vertices = geom.getVertices();
		const ofbx::Vec3* normals = geom.getNormals();
		const ofbx::Vec3* tangents = geom.getTangents();
		const ofbx::Vec4* colors = geom.getColors();
		const ofbx::Vec2* uvs = geom.getUVs();

		const int* materialId = geom.getMaterials();

		const int vertex_count = geom.getVertexCount();
		const int index_count = geom.getIndexCount();

		const bool has_normals = normals != nullptr;
		const bool has_tangents = tangents != nullptr;
		const bool has_colors = colors != nullptr;
		const bool has_texcoord = uvs != nullptr;

		if( vertex_count != index_count )
		{
			Error("TODO: FBX model not correct - index num not equal vertex num.");
			return {};
		}

		std::unordered_map<VertexMesh, uint32_t> uniqueVertices;
		Mesh tempMesh;
		for( int v_idx = 0; v_idx < vertex_count; ++v_idx )
		{
			ofbx::Vec3 v = vertices[v_idx];
			ofbx::Vec3 n = { 0.0f, 0.0f, 0.0f };
			if( has_normals ) n = normals[v_idx];
			ofbx::Vec3 t = { 0.0f, 0.0f, 0.0f };
			if( has_tangents ) n = tangents[v_idx];
			ofbx::Vec4 clr = { 1.0f, 1.0f, 1.0f, 1.0f };
			if( has_colors ) clr = colors[v_idx];
			ofbx::Vec2 uv = { 0.0f, 0.0f };
			if( has_texcoord ) uv = uvs[v_idx];

			VertexMesh vertex = {
				.position = { v.x, v.y, v.z },
				.normal = { n.x, n.y, n.z },
				//.tangents = { t.x, t.y, t.z },
				.color = { clr.x, clr.y, clr.z },
				.texCoord = { uv.x, uv.y }
			};

			if( uniqueVertices.count(vertex) == 0 )
			{
				uniqueVertices[vertex] = static_cast<uint32_t>(tempMesh.vertices.size());
				tempMesh.vertices.emplace_back(vertex);
			}
			tempMesh.indices.emplace_back(uniqueVertices[vertex]);
		}

		// Load Material
		const int mat_count = fbx_mesh.getMaterialCount();
		assert(mat_count == 1 && "not impl multi material");
		for( int mat_idx = 0; mat_idx < mat_count; ++mat_idx ) // по материалам
		{
			const ofbx::Material* mat = fbx_mesh.getMaterial(mat_idx);
			const ofbx::Texture* diffuseTexture = mat->getTexture(ofbx::Texture::DIFFUSE);

			ofbx::DataView filenamediffuseMap = diffuseTexture->getRelativeFileName();
			assert(filenamediffuseMap != "");
			char cfilenameDiffuse[260];
			filenamediffuseMap.toString(cfilenameDiffuse);
			std::string strfilenameDiffuse = std::string(cfilenameDiffuse);

			tempMesh.material.diffuseTexture = GetRenderSystem().CreateTexture2D(strfilenameDiffuse.c_str(), true);

			// TODO:
			//mat->getDiffuseColor();
			//mat->getAmbientColor();
			//mat->getDiffuseFactor();
		}

		Meshes.emplace_back(std::move(tempMesh));
	}

	// compute AABB
	computeSubMeshesAABB(Meshes);

	return createMeshBuffer(std::move(Meshes));
}
//-----------------------------------------------------------------------------
void GraphicsSystem::computeSubMeshesAABB(std::vector<Mesh>& meshes)
{
	for( size_t i = 0; i < meshes.size(); i++ )
	{
		for( size_t j = 0; j < meshes[i].vertices.size(); j++ )
		{
			meshes[i].globalAABB.Include(meshes[i].vertices[j].position);
		}
	}
}
//-----------------------------------------------------------------------------