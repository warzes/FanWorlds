#pragma once

#include "Transform.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

struct GJKProxy;
struct SimplexCache;

struct SimplexVertex
{
	Vec3 point1; // support vertex on proxy 1
	Vec3 point2; // support vertex on proxy 2
	Vec3 point; // minkowski vertex
	scalar weight; // barycentric coordinate for point
	uint32_t index1; // support 1 vertex index
	uint32_t index2; // support 2 vertex index
};

struct Simplex
{
	SimplexVertex m_vertices[4];
	uint32_t m_count;

	Vec3 GetSearchDirection(const Vec3& Q) const;
	Vec3 GetClosestPoint() const;
	void GetClosestPoints(Vec3* p1, Vec3* p2) const;

	void Solve2(const Vec3& Q);
	void Solve3(const Vec3& Q);
	void Solve4(const Vec3& Q);

	// Cache
	void ReadCache(const SimplexCache* cache,
		const Transform& xf1, const GJKProxy& proxy1,
		const Transform& xf2, const GJKProxy& proxy2);

	void WriteCache(SimplexCache* cache) const;

	scalar GetMetric() const;
};

// The output of the GJK algorithm.
// It contains the closest points between two proxies and their euclidean distance.
// If the distance is zero then the proxies are overlapping.
struct GJKOutput
{
	Vec3 point1; // closest point on proxy 1
	Vec3 point2; // closest point on proxy 2
	scalar distance; // euclidean distance between the closest points
	uint32_t iterations; // number of GJK iterations
};

// Find the closest points and distance between two proxies.
GJKOutput GJK(const Transform& xf1, const GJKProxy& proxy1, const Transform& xf2, const GJKProxy& proxy2, bool applyRadius);

///////////////////////////////////////////////////////////////////////////////////////////////////

// A cached simplex is used to improve the performance of the GJK when called more than once. 
// Make sure to set cache.count to zero before passing this structure as an argument to GJK when called for the first time.
struct SimplexCache
{
	scalar metric; // lenght or area or volume
	uint32_t iterations; // number of GJK iterations
	uint16_t count; // number of support vertices
	uint8_t index1[4]; // support vertices on proxy 1
	uint8_t index2[4]; // support vertices on proxy 2
};

// A feature pair contains the vertices of the features associated with the closest points.
struct GJKFeaturePair
{
	uint32_t index1[3]; // vertices on proxy 1
	uint32_t count1; // number of vertices on proxy 1
	uint32_t index2[3]; // vertices on proxy 2
	uint32_t count2; // number of vertices on proxy 2
};

// Identify the vertices of the features that the closest points between two 
// GJK proxies are contained on given a cached simplex.
// The GJK must have been called using the pair of proxies and cache.count must be < 4, that is, the proxies must not be overlapping.
GJKFeaturePair GetFeaturePair(const SimplexCache& cache);

// Find the closest points and distance between two proxies. 
// Assumes a simplex is given for increasing the performance of the algorithm when called more than once.
GJKOutput GJK(const Transform& xf1, const GJKProxy& proxy1, const Transform& xf2, const GJKProxy& proxy2, bool applyRadius, SimplexCache* cache);