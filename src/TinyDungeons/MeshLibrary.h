#pragma once

class MeshLibrary
{
public:
	void Create(RenderSystem& render, ModelRef model);
	void Destroy();

	std::vector<Mesh> meshes;
};