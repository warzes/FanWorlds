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

#ifndef B3_COLLIDE_H
#define B3_COLLIDE_H

#include <collision/gjk/gjk.h>
#include <collision/gjk/gjk_proxy.h>
#include <collision/sat/sat.h>
#include <collision/sat/sat_edge_and_hull.h>
#include <collision/sat/sat_vertex_and_hull.h>

class b3Shape;
class b3SphereShape;
class b3CapsuleShape;
class b3HullShape;
class b3MeshShape;

struct b3Manifold;

// A convex cache contains information used to exploit temporal 
// coherence of the contact generation algorithms between two shapes.
struct b3ConvexCache
{
	b3SimplexCache simplexCache; // last step simplex from the GJK
	b3FeatureCache featureCache; // last step result of the SAT
};

// Used for computing the distance between two generic shapes.
struct b3ShapeGJKProxy : public b3GJKProxy
{
	b3ShapeGJKProxy() { }

	b3ShapeGJKProxy(const b3Shape* shape, u32 index)
	{
		Set(shape, index);
	}

	void Set(const b3Shape* shape, u32 index);
};

// Test if two generic shapes are overlapping.
bool b3TestOverlap(const b3Transform& xf1, u32 index1, const b3Shape* shape1,
	const b3Transform& xf2, u32 index2, const b3Shape* shape2,
	b3ConvexCache* cache);

// Compute a manifold for two generic shapes except when one of them is a mesh.
void b3CollideShapeAndShape(b3Manifold& manifold, 
	const b3Transform& xf1, const b3Shape* shape1,
	const b3Transform& xf2, const b3Shape* shape2,
	b3ConvexCache* cache);

// Compute a manifold for two spheres.
void b3CollideSphereAndSphere(b3Manifold& manifold, 
	const b3Transform& xf1, const b3SphereShape* shape1, 
	const b3Transform& xf2, const b3SphereShape* shape2);

// Compute a manifold for a sphere and a hull.
void b3CollideSphereAndHull(b3Manifold& manifold, 
	const b3Transform& xf1, const b3SphereShape* shape1, 
	const b3Transform& xf2, const b3HullShape* shape2);

// Compute a manifold for a sphere and a capsule.
void b3CollideSphereAndCapsule(b3Manifold& manifold, 
	const b3Transform& xf1, const b3SphereShape* shape1, 
	const b3Transform& xf2, const b3CapsuleShape* shape2);

// Compute a manifold for two capsules.
void b3CollideCapsuleAndCapsule(b3Manifold& manifold, 
	const b3Transform& xf1, const b3CapsuleShape* shape1, 
	const b3Transform& xf2, const b3CapsuleShape* shape2);

// Compute a manifold for a capsule and a hull.
void b3CollideCapsuleAndHull(b3Manifold& manifold, 
	const b3Transform& xf1, const b3CapsuleShape* shape1, 
	const b3Transform& xf2, const b3HullShape* shape2);

// Compute a manifold for two hulls. 
void b3CollideHullAndHull(b3Manifold& manifold, 
	const b3Transform& xf1, const b3HullShape* shape1, 
	const b3Transform& xf2, const b3HullShape* shape2,
	b3ConvexCache* cache);

#endif