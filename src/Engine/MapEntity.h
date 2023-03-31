#pragma once

#include "MeshBase.h"

using EntityProperties = std::unordered_map<std::string, std::string>;

struct EntityBrushes
{
	std::unordered_map<std::string, std::vector<VertexBase>> TextureToVertices;
	std::vector<std::vector<physx::PxVec3>> Colliders;
};

struct EntityDefinition
{
	EntityProperties Properties;
	EntityBrushes Brushes;

	[[nodiscard]] const std::string &GetProperty(const std::string &key, const std::string &fallback = "") const;

	[[nodiscard]] bool GetPropertyInteger(const std::string &key, int &value) const;

	[[nodiscard]] bool GetPropertyFloat(const std::string &key, float &value) const;

	[[nodiscard]] bool GetPropertyColor(const std::string &key, glm::vec3 &value) const
	{
		return GetPropertyVec3(key, value);
	}

	[[nodiscard]] bool GetPropertyVector(const std::string &key, glm::vec3 &value) const
	{
		if( !GetPropertyVec3(key, value) ) return false;
		value = glm::vec3{ value.x, value.z, -value.y } / 32.0f; // convert from quake direction to haru direction
		return true;
	}

private:
	[[nodiscard]] bool GetPropertyVec3(const std::string &key, glm::vec3 &value) const;
};