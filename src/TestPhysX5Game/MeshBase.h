#pragma once

#include "Mesh.h"

struct VertexBase
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoord;

	static void SetupVertexArray(const GLuint vao)
	{
		SetupVertexArrayFloatsAttrib(vao, 0, 0, 3, offsetof(VertexBase, Position));
		SetupVertexArrayFloatsAttrib(vao, 1, 0, 3, offsetof(VertexBase, Normal));
		SetupVertexArrayFloatsAttrib(vao, 2, 0, 2, offsetof(VertexBase, TexCoord));
	}
};

using MeshBase = Mesh2<VertexBase>;