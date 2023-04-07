#pragma once

#include "MeshBase.h"
#include "Renderer.h"

class Texture2;

class FacesAndTexture
{
public:
	MOVE_ONLY(FacesAndTexture);
	FacesAndTexture() = delete;
	FacesAndTexture(SystemRef& systemRef, const std::vector<VertexBase> &vertices, const std::string &textureName);

	void Draw(Renderer &renderer, const glm::mat4 &modelMatrix);

private:
	MeshBase m_mesh;
	MoveOnly<Texture2 *> m_texture;
};