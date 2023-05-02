#pragma once

#include "Vec3.h"

// A GJK proxy encapsulates any convex hull to be used by the GJK.
struct GJKProxy
{
	const Vec3* vertices; // vertices in this proxy
	uint32_t vertexCount; // number of vertices
	scalar radius; // proxy radius
	Vec3 vertexBuffer[3]; // vertex buffer for convenience

	// Get the number of vertices in this proxy.
	uint32_t GetVertexCount() const;

	// Read an indexed vertex from this proxy.
	const Vec3& GetVertex(uint32_t index) const;

	// Get the support vertex index in a given direction.
	uint32_t GetSupportIndex(const Vec3& direction) const;

	// Convenience function.
	// Get the support vertex in a given direction.
	const Vec3& GetSupportVertex(const Vec3& direction) const;
};

inline uint32_t GJKProxy::GetVertexCount() const
{
	return vertexCount;
}

inline const Vec3& GJKProxy::GetVertex(uint32_t index) const
{
	assert(index < vertexCount);
	return vertices[index];
}

inline uint32_t GJKProxy::GetSupportIndex(const Vec3& d) const
{
	uint32_t maxIndex = 0;
	scalar maxProjection = Dot(d, vertices[maxIndex]);
	for (uint32_t i = 1; i < vertexCount; ++i)
	{
		scalar projection = Dot(d, vertices[i]);
		if (projection > maxProjection)
		{
			maxIndex = i;
			maxProjection = projection;
		}
	}
	return maxIndex;
}

inline const Vec3& GJKProxy::GetSupportVertex(const Vec3& d) const
{
	uint32_t index = GetSupportIndex(d);
	return vertices[index];
}