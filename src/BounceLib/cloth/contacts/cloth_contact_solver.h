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

#ifndef B3_CLOTH_CONTACT_SOLVER_H
#define B3_CLOTH_CONTACT_SOLVER_H

#include <common/math/mat22.h>
#include <common/math/mat33.h>

class b3StackAllocator;

class b3Particle;
class b3Body;

class b3ParticleBodyContact;
class b3ParticleTriangleContact;

struct b3ClothSolverBodyContactVelocityConstraint
{
	u32 indexA;
	float32 invMassA;
	b3Mat33 invIA;

	b3Body* bodyB;
	float32 invMassB;
	b3Mat33 invIB;

	float32 friction;

	b3Vec3 point;
	b3Vec3 rA;
	b3Vec3 rB;

	b3Vec3 normal;
	float32 normalMass;
	float32 normalImpulse;
	float32 velocityBias;

	b3Vec3 tangent1;
	b3Vec3 tangent2;
	b3Mat22 tangentMass;
	b3Vec2 tangentImpulse;
};

struct b3ClothSolverBodyContactPositionConstraint
{
	u32 indexA;
	float32 invMassA;
	b3Mat33 invIA;
	float32 radiusA;
	b3Vec3 localCenterA;

	b3Body* bodyB;
	float32 invMassB;
	b3Mat33 invIB;
	float32 radiusB;
	b3Vec3 localCenterB;

	b3Vec3 rA;
	b3Vec3 rB;

	b3Vec3 normalA;
	b3Vec3 localPointA;
	b3Vec3 localPointB;
};

struct b3ClothSolverTriangleContactVelocityConstraint
{
	u32 indexA;
	float32 invMassA;

	u32 indexB;
	float32 invMassB;
	u32 indexC;
	float32 invMassC;
	u32 indexD;
	float32 invMassD;
	
	float32 wB, wC, wD;

	b3Vec3 normal;
	float32 normalMass;
	float32 normalImpulse;

	float32 friction;

	b3Vec3 tangent1;
	b3Vec3 tangent2;
	float32 tangentMass1;
	float32 tangentMass2;
	float32 tangentImpulse1;
	float32 tangentImpulse2;
};

struct b3ClothSolverTriangleContactPositionConstraint
{
	u32 indexA;
	float32 invMassA;
	float32 radiusA;

	u32 indexB;
	float32 invMassB;
	u32 indexC;
	float32 invMassC;
	u32 indexD;
	float32 invMassD;
	float32 triangleRadius;

	float32 wB, wC, wD;
};

struct b3ClothContactSolverDef
{
	b3StackAllocator* allocator;
	
	b3Vec3* positions;
	b3Vec3* velocities;
	
	u32 bodyContactCount;
	b3ParticleBodyContact** bodyContacts;

	u32 triangleContactCount;
	b3ParticleTriangleContact** triangleContacts;
};

inline float32 b3MixFriction(float32 u1, float32 u2)
{
	return b3Sqrt(u1 * u2);
}

class b3ClothContactSolver
{
public:
	b3ClothContactSolver(const b3ClothContactSolverDef& def);
	~b3ClothContactSolver();

	void InitializeBodyContactConstraints();
	void InitializeTriangleContactConstraints();

	void WarmStartBodyContactConstraints();
	void WarmStartTriangleContactConstraints();

	void SolveBodyContactVelocityConstraints();
	void SolveTriangleContactVelocityConstraints();

	void StoreImpulses();

	bool SolveBodyContactPositionConstraints();
	bool SolveTriangleContactPositionConstraints();
protected:
	b3StackAllocator* m_allocator;

	b3Vec3* m_positions;
	b3Vec3* m_velocities;

	u32 m_bodyContactCount;
	b3ParticleBodyContact** m_bodyContacts;
	b3ClothSolverBodyContactVelocityConstraint* m_bodyVelocityConstraints;
	b3ClothSolverBodyContactPositionConstraint* m_bodyPositionConstraints;

	u32 m_triangleContactCount;
	b3ParticleTriangleContact** m_triangleContacts;
	b3ClothSolverTriangleContactVelocityConstraint* m_triangleVelocityConstraints;
	b3ClothSolverTriangleContactPositionConstraint* m_trianglePositionConstraints;
};

#endif