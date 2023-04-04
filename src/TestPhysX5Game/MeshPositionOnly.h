#pragma once

#include "Mesh.h"

struct VertexPositionOnly
{
	glm::vec3 Position;

	static void SetupVertexArray(const GLuint vao)
	{
		SetupVertexArrayFloatsAttrib(vao, 0, 0, 3, offsetof(VertexPositionOnly, Position));
	}
};

using MeshPositionOnly = Mesh2<VertexPositionOnly>;