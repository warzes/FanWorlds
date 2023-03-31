#include "stdafx.h"
#include "FacesAndTexture.h"
#include "Texture.h"
#include "Material.h"

FacesAndTexture::FacesAndTexture(SystemRef& systemRef, const std::vector<VertexBase> &vertices, const std::string &textureName)
	: m_mesh(vertices, GL_TRIANGLES),
	m_texture(Texture::LoadToCache(systemRef, "textures/" + textureName + ".png"))
{
}

void FacesAndTexture::Draw(Renderer &renderer, const glm::mat4 &modelMatrix)
{
	Material material{ m_texture };
	renderer.DrawMesh(m_mesh, modelMatrix, &material);
}