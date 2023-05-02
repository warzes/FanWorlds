#pragma once

#include "MicroMath.h"
#include "Vec3.h"
#include "Transform.h"

// A plane in constant normal form.
// dot(n, p) - d = 0.
class Plane
{
public:
	Plane() = default;
	// Set this plane from a normal and a signed distance from its origin.
	Plane(const Vec3& _normal, scalar _offset)
	{
		normal = _normal;
		offset = _offset;
	}
	// Set this plane from a normal and a point on the plane.
	Plane(const Vec3& _normal, const Vec3& _point)
	{
		normal = _normal;
		offset = Dot(_normal, _point);
	}

	// Compute this plane from three non-colinear points.
	Plane(const Vec3& A, const Vec3& B, const Vec3& C)
	{
		Vec3 N = Cross(B - A, C - A);
		normal = Normalize(N);
		offset = Dot(normal, A);
	}

	Vec3 normal;
	scalar offset;
};

// Transform a plane by a given frame.
inline Plane Mul(const Transform& T, const Plane& plane)
{
	Vec3 normal = Mul(T.rotation, plane.normal);
	return Plane(normal, plane.offset + Dot(normal, T.translation));
}

// Transform a plane by a given frame.
inline Plane operator*(const Transform& T, const Plane& plane)
{
	return Mul(T, plane);
}

// Compute the distance between a point and a plane.
inline scalar Distance(const Vec3& P, const Plane& plane)
{
	return Dot(plane.normal, P) - plane.offset;
}

// Project a point onto a normal plane.
inline Vec3 ClosestPointOnPlane(const Vec3& P, const Plane& plane)
{
	scalar fraction = Distance(P, plane);
	return P - fraction * plane.normal;
}