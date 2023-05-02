#pragma once

#include "Plane.h"

struct Hull;

///////////////////////////////////////////////////////////////////////////////////////////////////

struct FaceQuery
{
	uint32_t index;
	scalar separation;
};

scalar Project(const Hull* hull, const Plane& plane);

FaceQuery QueryFaceSeparation(const Transform& xf1, const Hull* hull1, const Transform& xf2, const Hull* hull2);

///////////////////////////////////////////////////////////////////////////////////////////////////

struct EdgeQuery
{
	uint32_t index1;
	uint32_t index2;
	scalar separation;
};

bool IsMinkowskiFace(const Vec3& A, const Vec3& B, const Vec3& B_x_A, const Vec3& C, const Vec3& D, const Vec3& D_x_C);

scalar Project(const Vec3& P1, const Vec3& E1, const Vec3& P2, const Vec3& E2, const Vec3& C1);

EdgeQuery QueryEdgeSeparation(const Transform& xf1, const Hull* hull1, const Transform& xf2, const Hull* hull2);


///////////////////////////////////////////////////////////////////////////////////////////////////

enum SATCacheType
{
	e_separation,
	e_overlap,
	e_empty,
};

enum SATFeatureType
{
	e_edge1, // an edge on hull 1 and an edge on hull 2
	e_face1, // a face on hull 1 and a vertex/edge/face on hull 2
	e_face2, // a face on hull 2 and a vertex/edge/face on hull 1
};

// A cached feature pair is used to improve the performance of the SAT when called more than once. 
struct SATFeaturePair
{
	SATCacheType state; // sat result
	SATFeatureType type; // feature pair type
	uint32_t index1; // feature index on hull 1
	uint32_t index2; // feature index on hull 2
};

inline SATFeaturePair MakeFeaturePair(SATCacheType state, SATFeatureType type, uint32_t index1, uint32_t index2)
{
	SATFeaturePair pair;
	pair.state = state;
	pair.type = type;
	pair.index1 = index1;
	pair.index2 = index2;
	return pair;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

struct FeatureCache
{
	// Default ctor does nothing for performance.
	FeatureCache() { }

	// Read the current state of the cache.
	// Return e_empty if neither a separation or penetration was detected.
	SATCacheType ReadState(const Transform& xf1, const Hull* hull1,
		const Transform& xf2, const Hull* hull2, scalar totalRadius);

	SATCacheType ReadEdge(const Transform& xf1, const Hull* hull1,
		const Transform& xf2, const Hull* hull2, scalar totalRadius);

	SATCacheType ReadFace(const Transform& xf1, const Hull* hull1,
		const Transform& xf2, const Hull* hull2, scalar totalRadius);

	// The feature pair.
	SATFeaturePair featurePair;
};