#pragma once

#include "Vec3.h"

class Sphere
{
public:
	Sphere() = default;
	Sphere(const Vec3& v, scalar r)
	{
		vertex = v;
		radius = r;
	}

	const Vec3& GetVertex(uint32_t index) const;
	uint32_t GetSupportVertex(const Vec3& direction) const;

	Vec3 vertex;
	scalar radius;
};

// Unit sphere centered at origin
extern const Sphere Sphere_identity;

inline const Vec3& Sphere::GetVertex(uint32_t /*index*/) const
{
	return vertex;
}

inline uint32_t Sphere::GetSupportVertex(const Vec3& /*direction*/) const
{
	return 0;
}