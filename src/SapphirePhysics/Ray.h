#pragma once

#include "Vec3.h"

// A ray in finite form.
// R(t) = O + t * n.
struct Ray
{
	Ray() = default;
	Ray(const Vec3& _origin, const Vec3& _direction, scalar _fraction) : origin(_origin), direction(_direction), fraction(_fraction) { }

	Ray(const Vec3& A, const Vec3& B)
	{
		origin = A;
		direction = Normalize(B - A);
		fraction = Distance(A, B);
	}

	// Return the begin point of this ray.
	Vec3 A() const { return origin; }

	// Return the end point of this ray.
	Vec3 B() const
	{
		return origin + fraction * direction;
	}

	Vec3 origin;
	Vec3 direction;
	scalar fraction;
};