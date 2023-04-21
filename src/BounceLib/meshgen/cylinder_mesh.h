/*
* Copyright (c) 2016-2019 Irlan Robson https://irlanrobson.github.io
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef CYM_MESH_H
#define CYM_MESH_H

#include <common/math/vec3.h>

// This structure represents a triangle mesh.
struct cymMesh
{
	cymMesh()
	{
		vertexCount = 0;
		vertices = nullptr;
		normals = nullptr;
		indexCount = 0;
		indices = nullptr;
	}

	~cymMesh()
	{
		b3Free(vertices);
		b3Free(normals);
		b3Free(indices);
	}

	u32 vertexCount; // number of unique vertices
	b3Vec3* vertices; // list of unique vertices
	b3Vec3* normals; // list of vertex normals
	u32 indexCount; // number of triangle vertex indices
	u32* indices; // list of triangle vertex index
};

// Create a unit cylinder along the y-axis given the number of segments.
// The number of segments must be greater than 2.
void cymCreateMesh(cymMesh& output, u32 segments);

#endif