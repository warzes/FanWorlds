#pragma once

#include "Vec3.h"

// Input for a ray cast.
struct RayCastInput
{
	Vec3 p1; // first point on segment
	Vec3 p2; // second point on segment
	scalar maxFraction; // maximum intersection
};

// Output of a ray cast.
struct RayCastOutput
{
	scalar fraction; // time of intersection on ray-segment
	Vec3 normal; // surface normal of intersection
};