#pragma once

#include "Plane.h"

struct Face
{
	uint32_t edge;
};

struct HalfEdge
{
	uint32_t origin;
	uint32_t twin;
	uint32_t face;
	uint32_t prev;
	uint32_t next;
};

struct Hull
{
	Vec3 centroid;
	uint32_t vertexCount;
	Vec3* vertices;
	uint32_t edgeCount;
	HalfEdge* edges;
	uint32_t faceCount;
	Face* faces;
	Plane* planes;

	const Vec3& GetVertex(uint32_t index) const;
	const HalfEdge* GetEdge(uint32_t index) const;
	const Face* GetFace(uint32_t index) const;
	const Plane& GetPlane(uint32_t index) const;

	uint32_t GetSupportVertex(const Vec3& direction) const;
	//uint32_t GetSupportEdge(const Vec3& direction) const;
	uint32_t GetSupportFace(const Vec3& direction) const;

	Plane GetEdgeSidePlane(uint32_t index) const;

	void Validate() const;
	void Validate(const Face* face) const;
	void Validate(const HalfEdge* edge) const;

	//void Dump() const;

	void Scale(const Vec3& scale);
	void Rotate(const Quat& rotation);
	void Translate(const Vec3& translation);

	// Scale -> Rotate -> Translate
	void GetTransform(const Transform& xf, const Vec3& scale);
};

inline HalfEdge MakeEdge(uint32_t origin, uint32_t twin, uint32_t face, uint32_t prev, uint32_t next)
{
	HalfEdge edge;
	edge.origin = origin;
	edge.twin = twin;
	edge.face = face;
	edge.prev = prev;
	edge.next = next;
	return edge;
}

inline const Vec3& Hull::GetVertex(uint32_t index) const
{
	return vertices[index];
}

inline const HalfEdge* Hull::GetEdge(uint32_t index) const
{
	return edges + index;
}

inline const Face* Hull::GetFace(uint32_t index) const
{
	return faces + index;
}

inline const Plane& Hull::GetPlane(uint32_t index) const
{
	return planes[index];
}

inline uint32_t Hull::GetSupportVertex(const Vec3& direction) const
{
	uint32_t maxIndex = 0;
	scalar maxProjection = Dot(direction, vertices[maxIndex]);
	for (uint32_t i = 1; i < vertexCount; ++i)
	{
		scalar projection = Dot(direction, vertices[i]);
		if (projection > maxProjection)
		{
			maxIndex = i;
			maxProjection = projection;
		}
	}
	return maxIndex;
}

inline uint32_t Hull::GetSupportFace(const Vec3& direction) const
{
	uint32_t maxIndex = 0;
	scalar maxProjection = Dot(direction, planes[maxIndex].normal);
	for (uint32_t i = 1; i < faceCount; ++i)
	{
		scalar projection = Dot(direction, planes[i].normal);
		if (projection > maxProjection)
		{
			maxIndex = i;
			maxProjection = projection;
		}
	}
	return maxIndex;
}

inline Plane Hull::GetEdgeSidePlane(uint32_t index) const
{
	const HalfEdge* edge = edges + index;
	const HalfEdge* twin = edges + edge->twin;
	const Plane* facePlane = planes + edge->face;

	Vec3 P = vertices[edge->origin];
	Vec3 Q = vertices[twin->origin];
	Vec3 E = Q - P;
	Vec3 D = Cross(E, facePlane->normal);

	Plane plane;
	plane.normal = Normalize(D);
	plane.offset = Dot(plane.normal, P);
	return plane;
}