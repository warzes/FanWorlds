#include "stdafx.h"
#include "Hull.h"
#include "Common.h"

void Hull::Validate() const
{
	for (uint32_t i = 0; i < faceCount; ++i)
	{
		Validate(faces + i);
	}

	for (uint32_t i = 0; i < edgeCount; i += 2)
	{
		const HalfEdge* edge = edges + i;
		const HalfEdge* twin = edges + i + 1;

		Vec3 A = vertices[edge->origin];
		Vec3 B = vertices[twin->origin];

		// Ensure each edge has non-zero length.
		assert(DistanceSquared(A, B) > SE_LINEAR_SLOP * SE_LINEAR_SLOP);
	}
}

void Hull::Validate(const Face* face) const
{
	uint32_t n = 0;
	const HalfEdge* begin = GetEdge(face->edge);
	const HalfEdge* edge = begin;
	do
	{
		assert(GetFace(edge->face) == face);
		++n;
		edge = edges + edge->next;
	} while (edge != begin);

	assert(n >= 3);
	Validate(edges + face->edge);
}

void Hull::Validate(const HalfEdge* e) const
{
	uint32_t edgeIndex = (uint32_t)(e - edges);

	const HalfEdge* twin = edges + e->twin;

	// Each edge must be followed by its twin.
	assert(glm::abs(twin - e) == 1);
	assert(twin->twin == edgeIndex);

	uint32_t count = 0;

	const HalfEdge* begin = e;
	do
	{
		assert(count < edgeCount);
		++count;

		const HalfEdge* next = edges + e->next;
		e = edges + next->twin;
	} while (e != begin);
}

void Hull::Scale(const Vec3& scale)
{
	// https://irlanrobson.github.io/2019/10/01/how-to-transform-a-plane,-with-scale/
	assert(scale.x > scalar(0));
	assert(scale.y > scalar(0));
	assert(scale.z > scalar(0));

	for (uint32_t i = 0; i < vertexCount; ++i)
	{
		vertices[i] = Mul(scale, vertices[i]);
	}

	// M = S
	// M^-1 = S^-1
	// (M^-1)^T = (S^-1)^T = S^-1
	Vec3 inv_scale;
	inv_scale.x = scalar(1) / scale.x;
	inv_scale.y = scalar(1) / scale.y;
	inv_scale.z = scalar(1) / scale.z;

	for (uint32_t i = 0; i < faceCount; ++i)
	{
		Plane oldPlane = planes[i];

		Plane newPlane;
		newPlane.normal = Mul(inv_scale, oldPlane.normal);

		assert(newPlane.normal.LengthSq() > scalar(0));
		scalar len = newPlane.normal.Length();
		newPlane.normal.Normalize();

		newPlane.offset = oldPlane.offset / len;

		planes[i] = newPlane;
	}

	centroid = Mul(scale, centroid);
}

void Hull::Rotate(const Quat& rotation)
{
	for (uint32_t i = 0; i < vertexCount; ++i)
	{
		vertices[i] = Mul(rotation, vertices[i]);
	}

	for (uint32_t i = 0; i < faceCount; ++i)
	{
		planes[i].normal = Mul(rotation, planes[i].normal);
	}
}

void Hull::Translate(const Vec3& translation)
{
	for (uint32_t i = 0; i < vertexCount; ++i)
	{
		vertices[i] += translation;
	}

	for (uint32_t i = 0; i < faceCount; ++i)
	{
		planes[i].offset += Dot(planes[i].normal, translation);
	}
}

void Hull::GetTransform(const Transform& xf, const Vec3& scale)
{
	// https://irlanrobson.github.io/2019/10/01/how-to-transform-a-plane,-with-scale/
	assert(scale.x > scalar(0));
	assert(scale.y > scalar(0));
	assert(scale.z > scalar(0));

	for (uint32_t i = 0; i < vertexCount; ++i)
	{
		vertices[i] = Mul(xf.rotation, Mul(scale, vertices[i])) + xf.translation;
	}

	// M = R * S
	// M^-1 = S^-1 * R^-1
	// (M^-1)^T = (R^-1)^T * (S^-1)^T = R * S^-1
	Vec3 inv_scale;
	inv_scale.x = scalar(1) / scale.x;
	inv_scale.y = scalar(1) / scale.y;
	inv_scale.z = scalar(1) / scale.z;

	for (uint32_t i = 0; i < faceCount; ++i)
	{
		Plane oldPlane = planes[i];

		Plane newPlane;
		newPlane.normal = Mul(xf.rotation, Mul(inv_scale, oldPlane.normal));

		assert(newPlane.normal.LengthSq() > scalar(0));
		scalar len = newPlane.normal.Length();
		newPlane.normal.Normalize();

		newPlane.offset = oldPlane.offset / len + Dot(newPlane.normal, xf.translation);

		planes[i] = newPlane;
	}

	centroid = Mul(xf.rotation, Mul(scale, centroid)) + xf.translation;
}

//void Hull::Dump() const
//{
//	Log("Vec3 vertices[%d]\n", vertexCount);
//	Log("HalfEdge edges[%d]\n", edgeCount);
//	Log("Face faces[%d]\n", faceCount);
//	Log("Plane planes[%d]\n", faceCount);
//
//	for (uint32_t i = 0; i < vertexCount; ++i)
//	{
//		Vec3 v = vertices[i];
//
//		Log("vertices[%d] = Vec3(%.20f, %.20f, %.20f);\n", i, v.x, v.y, v.z);
//	}
//
//	Log("\n");
//
//	for (uint32_t i = 0; i < edgeCount; ++i)
//	{
//		HalfEdge* he = edges + i;
//
//		Log("edges[%d] = MakeEdge(%d, %d, %d, %d, %d);\n", i, he->origin, he->twin, he->face, he->prev, he->next);
//	}
//
//	Log("\n");
//
//	for (uint32_t i = 0; i < faceCount; ++i)
//	{
//		Log("faces[%d].edge = %d;\n", i, faces[i].edge);
//	}
//
//	Log("\n");
//
//	for (uint32_t i = 0; i < faceCount; ++i)
//	{
//		Plane plane = planes[i];
//
//		Log("planes[%d].normal = Vec3(%.20f, %.20f, %.20f);\n", i, plane.normal.x, plane.normal.y, plane.normal.z);
//		Log("planes[%d].offset = %.20f;\n", i, plane.offset);
//	}
//
//	Log("centroid = Vec3(%.20f, %.20f, %.20f);\n", centroid.x, centroid.y, centroid.z);
//}