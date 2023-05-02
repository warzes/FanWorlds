#include "stdafx.h"
#include "sat.h"
#include "Hull.h"
#include "Common.h"

// Implementation of the SAT (Separating Axis Test) for convex hulls. Thanks to Dirk Gregorius for his presentation at GDC 2013.

scalar Project(const Hull* hull, const Plane& plane)
{
	Vec3 support = hull->GetVertex(hull->GetSupportVertex(-plane.normal));
	return Distance(support, plane);
}

// Query minimum separation distance and axis of the first hull planes.
FaceQuery QueryFaceSeparation(const Transform& xf1, const Hull* hull1, const Transform& xf2, const Hull* hull2)
{
	// Perform computations in the local space of the second hull.
	Transform xf = MulT(xf2, xf1);

	// Here greater means less than since is a signed distance.
	uint32_t maxIndex = 0;
	scalar maxSeparation = -SE_MAX_SCALAR;

	for (uint32_t i = 0; i < hull1->faceCount; ++i)
	{
		Plane plane = xf * hull1->GetPlane(i);
		scalar separation = Project(hull2, plane);
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

bool IsMinkowskiFace(const Vec3& A, const Vec3& B, const Vec3& B_x_A, const Vec3& C, const Vec3& D, const Vec3& D_x_C)
{
	scalar ADC = Dot(A, D_x_C);
	scalar BDC = Dot(B, D_x_C);

	scalar CBA = Dot(C, B_x_A);
	scalar DBA = Dot(D, B_x_A);

	return CBA * DBA < scalar(0) && // Test arc CD against AB plane.
		ADC * BDC < scalar(0) && // Test arc AB against DC plane.
		CBA * BDC > scalar(0); // Test if arcs AB and CD are on the same hemisphere.
}

scalar Project(const Vec3& P1, const Vec3& Q1, const Vec3& E1, const Vec3& P2, const Vec3& E2, const Vec3& C1)
{
	scalar L1 = Length(E1);
	assert(L1 > SE_LINEAR_SLOP);
	if (L1 < SE_LINEAR_SLOP)
	{
		return -SE_MAX_SCALAR;
	}

	scalar L2 = Length(E2);
	assert(L2 > SE_LINEAR_SLOP);
	if (L2 < SE_LINEAR_SLOP)
	{
		return -SE_MAX_SCALAR;
	}

	// Skip over almost parallel edges.
	const scalar kTol = scalar(0.005);

	Vec3 E1_x_E2 = Cross(E1, E2);
	scalar L = Length(E1_x_E2);
	if (L < kTol * L1 * L2)
	{
		return -SE_MAX_SCALAR;
	}

	Vec3 E1C = scalar(0.5) * (P1 + Q1);

	// Ensure consistent normal orientation to hull B.
	Vec3 N = (scalar(1) / L) * E1_x_E2;
	if (Dot(N, E1C - C1) < scalar(0))
	{
		N = -N;
	}

	// d = dot(N, P2) - offset = dot(N, P2) - dot(N, P1) = dot(N, P2 - P1) 
	return Dot(N, P2 - P1);
}

EdgeQuery QueryEdgeSeparation(const Transform& xf1, const Hull* hull1, const Transform& xf2, const Hull* hull2)
{
	// Query minimum separation distance and axis of the first hull planes.
	// Perform computations in the local space of the second hull.
	Transform xf = MulT(xf2, xf1);
	Vec3 C1 = xf * hull1->centroid;

	uint32_t maxIndex1 = 0;
	uint32_t maxIndex2 = 0;
	scalar maxSeparation = -SE_MAX_SCALAR;

	// Loop through the first hull's unique edges.
	for (uint32_t i = 0; i < hull1->edgeCount; i += 2)
	{
		const HalfEdge* edge1 = hull1->GetEdge(i);
		const HalfEdge* twin1 = hull1->GetEdge(i + 1);

		assert(edge1->twin == i + 1 && twin1->twin == i);

		Vec3 P1 = xf * hull1->GetVertex(edge1->origin);
		Vec3 Q1 = xf * hull1->GetVertex(twin1->origin);
		Vec3 E1 = Q1 - P1;

		// The Gauss Map of edge 1.
		Vec3 U1 = Mul(xf.rotation, hull1->GetPlane(edge1->face).normal);
		Vec3 V1 = Mul(xf.rotation, hull1->GetPlane(twin1->face).normal);

		// Loop through the second hull's unique edges.
		for (uint32_t j = 0; j < hull2->edgeCount; j += 2)
		{
			const HalfEdge* edge2 = hull2->GetEdge(j);
			const HalfEdge* twin2 = hull2->GetEdge(j + 1);

			assert(edge2->twin == j + 1 && twin2->twin == j);

			Vec3 P2 = hull2->GetVertex(edge2->origin);
			Vec3 Q2 = hull2->GetVertex(twin2->origin);
			Vec3 E2 = Q2 - P2;

			// The Gauss Map of edge 2.
			Vec3 U2 = hull2->GetPlane(edge2->face).normal;
			Vec3 V2 = hull2->GetPlane(twin2->face).normal;

			// Negate the Gauss Map 2 for account for the MD.
			if (IsMinkowskiFace(U1, V1, -E1, -U2, -V2, -E2))
			{
				scalar separation = Project(P1, Q1, E1, P2, E2, C1);
				if (separation > maxSeparation)
				{
					maxSeparation = separation;
					maxIndex1 = i;
					maxIndex2 = j;
				}
			}
		}
	}

	EdgeQuery out;
	out.index1 = maxIndex1;
	out.index2 = maxIndex2;
	out.separation = maxSeparation;
	return out;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

SATCacheType FeatureCache::ReadState(const Transform& xf1, const Hull* hull1, const Transform& xf2, const Hull* hull2, scalar totalRadius)
{
	// If the cache was empty or flushed choose an arbitrary feature pair.
	if (featurePair.state == SATCacheType::e_empty)
	{
		featurePair = MakeFeaturePair(SATCacheType::e_separation, SATFeatureType::e_face1, 0, 0);
	}

	switch (featurePair.type)
	{
	case SATFeatureType::e_edge1:
	{
		return ReadEdge(xf1, hull1, xf2, hull2, totalRadius);
	}
	case SATFeatureType::e_face1:
	{
		return ReadFace(xf1, hull1, xf2, hull2, totalRadius);
	}
	case SATFeatureType::e_face2:
	{
		return ReadFace(xf2, hull2, xf1, hull1, totalRadius);
	}
	default:
	{
		return SATCacheType::e_empty;
	}
	}
}

SATCacheType FeatureCache::ReadFace(const Transform& xf1, const Hull* hull1, const Transform& xf2, const Hull* hull2, scalar totalRadius)
{
	// Perform computations in the local space of the second hull.
	Transform xf = MulT(xf2, xf1);
	Plane plane = xf * hull1->GetPlane(featurePair.index1);
	scalar separation = Project(hull2, plane);
	if (separation > totalRadius)
	{
		return e_separation;
	}
	return e_overlap;
}

SATCacheType FeatureCache::ReadEdge(const Transform& xf1, const Hull* hull1, const Transform& xf2, const Hull* hull2, scalar totalRadius)
{
	uint32_t i = featurePair.index1;
	uint32_t j = featurePair.index2;

	// Query minimum separation distance and axis of the first hull planes.
	// Perform computations in the local space of the second hull.
	Transform xf = MulT(xf2, xf1);
	Vec3 C1 = xf * hull1->centroid;

	const HalfEdge* edge1 = hull1->GetEdge(i);
	const HalfEdge* twin1 = hull1->GetEdge(i + 1);

	assert(edge1->twin == i + 1 && twin1->twin == i);

	Vec3 P1 = xf * hull1->GetVertex(edge1->origin);
	Vec3 Q1 = xf * hull1->GetVertex(twin1->origin);
	Vec3 E1 = Q1 - P1;

	// The Gauss Map of edge 1.
	Vec3 U1 = Mul(xf.rotation, hull1->GetPlane(edge1->face).normal);
	Vec3 V1 = Mul(xf.rotation, hull1->GetPlane(twin1->face).normal);

	const HalfEdge* edge2 = hull2->GetEdge(j);
	const HalfEdge* twin2 = hull2->GetEdge(j + 1);

	assert(edge2->twin == j + 1 && twin2->twin == j);

	Vec3 P2 = hull2->GetVertex(edge2->origin);
	Vec3 Q2 = hull2->GetVertex(twin2->origin);
	Vec3 E2 = Q2 - P2;

	// The Gauss Map of edge 2.
	Vec3 U2 = hull2->GetPlane(edge2->face).normal;
	Vec3 V2 = hull2->GetPlane(twin2->face).normal;

	// Negate the Gauss Map 2 for account for the MD.
	if (IsMinkowskiFace(U1, V1, -E1, -U2, -V2, -E2))
	{
		scalar separation = Project(P1, Q1, E1, P2, E2, C1);
		if (separation > totalRadius)
		{
			return SATCacheType::e_separation;
		}
		else
		{
			return SATCacheType::e_overlap;
		}
	}

	// We can't determine the cache type therefore must run SAT.
	return SATCacheType::e_empty;
}