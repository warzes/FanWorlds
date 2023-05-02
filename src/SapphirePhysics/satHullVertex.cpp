#include "stdafx.h"
#include "satHullVertex.h"
#include "Hull.h"
#include "Sphere.h"

scalar ProjectVertex(const Sphere* hull, const Plane& plane)
{
	Vec3 support = hull->GetVertex(hull->GetSupportVertex(-plane.normal));
	return Distance(support, plane);
}

FaceQuery QueryFaceSeparation(const Transform& xf1, const Hull* hull1, const Transform& xf2, const Sphere* hull2)
{
	// Perform computations in the local space of the first hull.
	Vec3 support = MulT(xf1, Mul(xf2, hull2->vertex));

	uint32_t maxIndex = 0;
	scalar maxSeparation = -SE_MAX_SCALAR;

	for (uint32_t i = 0; i < hull1->faceCount; ++i)
	{
		Plane plane = hull1->GetPlane(i);
		scalar separation = Distance(support, plane);

		if (separation > maxSeparation)
		{
			maxIndex = i;
			maxSeparation = separation;
		}
	}

	FaceQuery out;
	out.index = maxIndex;
	out.separation = maxSeparation;
	return out;
}
