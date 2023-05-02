#include "stdafx.h"
#include "satHullEdge.h"
#include "Hull.h"
#include "Capsule.h"
#include "Common.h"

scalar ProjectEdge(const Capsule* hull, const Plane& plane)
{
	Vec3 support = hull->GetVertex(hull->GetSupportVertex(-plane.normal));
	return Distance(support, plane);
}

FaceQuery QueryFaceSeparation(const Transform& xf1, const Hull* hull1, const Transform& xf2, const Capsule* hull2)
{
	// Perform computations in the local space of the first hull.
	Transform xf = MulT(xf1, xf2);

	Capsule localHull2;
	localHull2.vertex1 = xf * hull2->vertex1;
	localHull2.vertex2 = xf * hull2->vertex2;

	// Here greater means less than since is a signed distance.
	uint32_t maxIndex = 0;
	scalar maxSeparation = -SE_MAX_SCALAR;

	for (uint32_t i = 0; i < hull1->faceCount; ++i)
	{
		Plane plane = hull1->GetPlane(i);
		scalar separation = ProjectEdge(&localHull2, plane);
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

// Qualify the two hull normals against the plane of the ring of the capsule.
bool IsMinkowskiFaceEdge(const Vec3& N, const Vec3& C, const Vec3& D)
{
	return Dot(N, C) * Dot(N, D) < scalar(0);
}

scalar ProjectEdge(const Vec3& P1, const Vec3& E1, const Vec3& C1, const Vec3& P2, const Vec3& E2)
{
	scalar L1 = Length(E1);
	if (L1 < SE_LINEAR_SLOP)
	{
		return -SE_MAX_SCALAR;
	}

	scalar L2 = Length(E2);
	if (L2 < SE_LINEAR_SLOP)
	{
		return -SE_MAX_SCALAR;
	}

	// Skip over almost parallel edges.
	const scalar kTol = 0.005f;

	Vec3 E1_x_E2 = Cross(E1, E2);
	scalar L = Length(E1_x_E2);
	if (L < kTol * L1 * L2)
	{
		return -SE_MAX_SCALAR;
	}

	// Ensure consistent normal orientation to hull B.
	Vec3 N = (scalar(1) / L) * E1_x_E2;
	if (Dot(N, P1 - C1) < scalar(0))
	{
		N = -N;
	}

	// d = dot(N, P2) - offset = dot(N, P2) - dot(N, P1) = dot(N, P2 - P1) 
	return Dot(N, P2 - P1);
}

EdgeQuery QueryEdgeSeparation(const Transform& xf1, const Hull* hull1, const Transform& xf2, const Capsule* hull2)
{
	// Query minimum edge separation.
	uint32_t maxIndex = 0;
	scalar maxSeparation = -SE_MAX_SCALAR;

	// Perform computations in the local space of the first hull.
	Transform xf = MulT(xf1, xf2);
	Vec3 C1 = hull1->centroid;

	Vec3 P2 = xf * hull2->vertex1;
	Vec3 Q2 = xf * hull2->vertex2;
	Vec3 E2 = Q2 - P2;

	for (uint32_t i = 0; i < hull1->edgeCount; i += 2)
	{
		const HalfEdge* edge1 = hull1->GetEdge(i);
		const HalfEdge* twin1 = hull1->GetEdge(i + 1);

		assert(edge1->twin == i + 1 && twin1->twin == i);

		Vec3 P1 = hull1->GetVertex(edge1->origin);
		Vec3 Q1 = hull1->GetVertex(twin1->origin);
		Vec3 E1 = Q1 - P1;

		Vec3 U1 = hull1->GetPlane(edge1->face).normal;
		Vec3 V1 = hull1->GetPlane(twin1->face).normal;

		if (IsMinkowskiFaceEdge(E2, U1, V1))
		{
			scalar separation = ProjectEdge(P1, E1, C1, P2, E2);
			if (separation > maxSeparation)
			{
				maxSeparation = separation;
				maxIndex = i;
			}
		}
	}

	EdgeQuery out;
	out.index1 = maxIndex;
	out.index2 = SE_MAX_U32;
	out.separation = maxSeparation;
	return out;
}