#pragma once

#include "Hull.h"

struct TriangleHull : public Hull
{
	Vec3 triangleVertices[3];
	HalfEdge triangleEdges[6];
	Face triangleFaces[2];
	Plane trianglePlanes[2];

	TriangleHull() = default;

	TriangleHull(const Vec3& A, const Vec3& B, const Vec3& C)
	{
		Set(A, B, C);
	}

	void Set(const Vec3& A, const Vec3& B, const Vec3& C)
	{
		centroid = (A + B + C) / scalar(3);

		triangleVertices[0] = A;
		triangleVertices[1] = B;
		triangleVertices[2] = C;

		// Each edge must be followed by its twin.
		triangleEdges[0] = MakeEdge(0, 1, 0, 4, 2); // Face 0 - Edge 0
		triangleEdges[2] = MakeEdge(1, 3, 0, 0, 4); // Face 0 - Edge 1
		triangleEdges[4] = MakeEdge(2, 5, 0, 2, 0); // Face 0 - Edge 2

		triangleEdges[1] = MakeEdge(1, 0, 1, 3, 5); // Face 1 - Edge 0
		triangleEdges[3] = MakeEdge(2, 2, 1, 5, 1); // Face 1 - Edge 1
		triangleEdges[5] = MakeEdge(0, 4, 1, 1, 3); // Face 1 - Edge 2

		triangleFaces[0].edge = 0;
		triangleFaces[1].edge = 1;

		Vec3 N = Cross(B - A, C - A);
		N = Normalize(N);

		trianglePlanes[0] = Plane(N, centroid);
		trianglePlanes[1] = Plane(-N, centroid);

		vertices = triangleVertices;
		vertexCount = 3;
		edges = triangleEdges;
		edgeCount = 6;
		faces = triangleFaces;
		planes = trianglePlanes;
		faceCount = 2;
	}
};