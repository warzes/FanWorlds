#include "stdafx.h"
#include "MeshLibrary.h"
//-----------------------------------------------------------------------------
void MeshLibrary::Create(RenderSystem& render, ModelRef model)
{
	meshes.resize(model->subMeshes.size());
	for( size_t i = 0; i < model->subMeshes.size(); i++ )
	{
		meshes[i] = std::move(model->subMeshes[i]);
	}
	model = 0;

	for( size_t i = 0; i < meshes.size(); i++ )
	{
		Mesh& mesh = meshes[i];
		const glm::vec3 offset = mesh.GetMeshPos();
		mesh.globalAABB.min -= offset;
		mesh.globalAABB.max -= offset;
		for( size_t j = 0; j < mesh.vertices.size(); j++ )
		{
			VertexMesh& vertex = mesh.vertices[j];
			vertex.position -= offset;
		}

		render.UpdateBuffer(mesh.geometry->GetVBO(), 0, mesh.vertices.size(), sizeof(mesh.vertices[0]), mesh.vertices.data());
	}
}
//-----------------------------------------------------------------------------
void MeshLibrary::Destroy()
{
	// TODO: все удаляет?
	meshes.clear();
}
//-----------------------------------------------------------------------------