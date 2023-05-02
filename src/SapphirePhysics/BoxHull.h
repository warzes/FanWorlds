#pragma once

#include "Hull.h"

struct BoxHull : public Hull
{
	Vec3 boxVertices[8];
	HalfEdge boxEdges[24];
	Face boxFaces[6];
	Plane boxPlanes[6];

	BoxHull() = default;

	// Construct this box from three extents and centered at the origin.
	BoxHull(scalar ex, scalar ey, scalar ez)
	{
		SetExtents(ex, ey, ez);
	}

	// Set this box to the unit box centered at the origin.
	void SetIdentity()
	{
		boxVertices[0] = Vec3(-1, -1, 1);
		boxVertices[1] = Vec3(1, -1, -1);
		boxVertices[2] = Vec3(1, 1, 1);
		boxVertices[3] = Vec3(-1, 1, -1);
		boxVertices[4] = Vec3(1, 1, -1);
		boxVertices[5] = Vec3(-1, 1, 1);
		boxVertices[6] = Vec3(1, -1, 1);
		boxVertices[7] = Vec3(-1, -1, -1);

		boxEdges[0] = MakeEdge(0, 1, 0, 6, 2);
		boxEdges[1] = MakeEdge(5, 0, 1, 12, 8);
		boxEdges[2] = MakeEdge(5, 3, 0, 0, 4);
		boxEdges[3] = MakeEdge(3, 2, 5, 19, 13);
		boxEdges[4] = MakeEdge(3, 5, 0, 2, 6);
		boxEdges[5] = MakeEdge(7, 4, 3, 15, 18);
		boxEdges[6] = MakeEdge(7, 7, 0, 4, 0);
		boxEdges[7] = MakeEdge(0, 6, 2, 9, 14);
		boxEdges[8] = MakeEdge(0, 9, 1, 1, 10);
		boxEdges[9] = MakeEdge(6, 8, 2, 16, 7);
		boxEdges[10] = MakeEdge(6, 11, 1, 8, 12);
		boxEdges[11] = MakeEdge(2, 10, 4, 22, 17);
		boxEdges[12] = MakeEdge(2, 13, 1, 10, 1);
		boxEdges[13] = MakeEdge(5, 12, 5, 3, 23);
		boxEdges[14] = MakeEdge(7, 15, 2, 7, 16);
		boxEdges[15] = MakeEdge(1, 14, 3, 20, 5);
		boxEdges[16] = MakeEdge(1, 17, 2, 14, 9);
		boxEdges[17] = MakeEdge(6, 16, 4, 11, 21);
		boxEdges[18] = MakeEdge(3, 19, 3, 5, 20);
		boxEdges[19] = MakeEdge(4, 18, 5, 23, 3);
		boxEdges[20] = MakeEdge(4, 21, 3, 18, 15);
		boxEdges[21] = MakeEdge(1, 20, 4, 17, 22);
		boxEdges[22] = MakeEdge(4, 23, 4, 21, 11);
		boxEdges[23] = MakeEdge(2, 22, 5, 13, 19);

		boxFaces[0].edge = 0;
		boxFaces[1].edge = 8;
		boxFaces[2].edge = 7;
		boxFaces[3].edge = 15;
		boxFaces[4].edge = 21;
		boxFaces[5].edge = 23;

		boxPlanes[0].normal = Vec3(-1, 0, 0);
		boxPlanes[0].offset = 1;

		boxPlanes[1].normal = Vec3(0, 0, 1);
		boxPlanes[1].offset = 1;

		boxPlanes[2].normal = Vec3(0, -1, 0);
		boxPlanes[2].offset = 1;

		boxPlanes[3].normal = Vec3(0, 0, -1);
		boxPlanes[3].offset = 1;

		boxPlanes[4].normal = Vec3(1, 0, 0);
		boxPlanes[4].offset = 1;

		boxPlanes[5].normal = Vec3(0, 1, 0);
		boxPlanes[5].offset = 1;

		centroid = Vec3(0, 0, 0);
		vertices = boxVertices;
		vertexCount = 8;
		edges = boxEdges;
		edgeCount = 24;
		faces = boxFaces;
		planes = boxPlanes;
		faceCount = 6;

		Validate();
	}

	// Set this box from three extents and centered and aligned at the origin.
	void SetExtents(scalar ex, scalar ey, scalar ez)
	{
		SetIdentity();

		Vec3 scale(ex, ey, ez);
		Scale(scale);
	}
};

extern const BoxHull BoxHull_identity;