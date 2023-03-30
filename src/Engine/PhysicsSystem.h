#pragma once

#include "BaseClass.h"

namespace physx
{
	class PxFoundation;
	class PxPvdTransport;
	class PxPvd;
	class PxPhysics;
	class PxCooking;
	class PxConvexMesh;
	class PxTriangleMesh;
}

class PhysicsSystem : public BaseClass
{
	friend class EngineDevice;
	friend class PhysicsScene;
public:
	bool Create();
	void Destroy();


	physx::PxConvexMesh* CreateConvexMesh(
		physx::PxU32 count, 
		const physx::PxVec3 *vertices,
		// The number of vertices and faces of a convex mesh in PhysX is limited to 255.
		physx::PxU16 vertexLimit = 255
	);

	physx::PxTriangleMesh *CreateTriangleMesh(
		physx::PxU32 count,
		const physx::PxVec3 *vertices
	);
private:
	//PhysicsSystem(PhysicsSystem&&) = delete;
	//PhysicsSystem(const PhysicsSystem&) = delete;
	//PhysicsSystem& operator=(PhysicsSystem&&) = delete;
	//PhysicsSystem& operator=(const PhysicsSystem&) = delete;

	physx::PxFoundation *m_foundation = nullptr;
	physx::PxPvdTransport *m_pvdTransport = nullptr;
	physx::PxPvd *m_pvd = nullptr;
	physx::PxPhysics *m_physics = nullptr;
	physx::PxCooking *m_cooking = nullptr;
};