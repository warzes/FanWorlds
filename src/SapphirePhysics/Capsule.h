#pragma once

#include "Vec3.h"

struct Capsule
{
	Vec3 vertex1, vertex2;
	scalar radius;

	Capsule() = default;
	Capsule(const Vec3& v1, const Vec3& v2, scalar r)
	{
		vertex1 = v1;
		vertex2 = v2;
		radius = r;
	}

	const Vec3& GetVertex(uint32_t index) const;
	uint32_t GetSupportVertex(const Vec3& direction) const;
};

// Unit capsule centered at the origin
extern const Capsule Capsule_identity;

inline const Vec3& Capsule::GetVertex(uint32_t index) const
{
	return (&vertex1)[index];
}

inline uint32_t Capsule::GetSupportVertex(const Vec3& d) const
{
	if (Dot(d, vertex1) > Dot(d, vertex2))
	{
		return 0;
	}
	return 1;
}