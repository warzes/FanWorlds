#pragma once

#include "MeshBase.h"

std::vector<VertexBase> CreateBox(const glm::vec3 &min, const glm::vec3 &max);

static std::vector<VertexBase> CreateBox(const glm::vec3 &halfSize)
{
	return CreateBox(-halfSize, halfSize);
}