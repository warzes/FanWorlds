#pragma once

#include "BaseClass.h"
#include "PhysicsSystem.h"

namespace physx
{
	class PxFoundation;
	class PxPvdTransport;
	class PxPvd;
	class PxPhysics;
	class PxCooking;
	class PxMaterial;
	class PxConvexMesh;
	class PxTriangleMesh;
}

class PhysicsScene;

class PhysicsSystem : public BaseClass
{
	friend class EngineDevice;
	friend class PhysicsScene;
public:
	bool Create();
	void Destroy();

	physx::PxMaterial* CreateMaterial(physx::PxReal staticFriction, physx::PxReal dynamicFriction, physx::PxReal restitution);

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

	void AddScene(PhysicsScene* scene) { m_physicsScene = scene; }
	PhysicsScene *GetPhysicsScene() { assert(m_physicsScene); return m_physicsScene; }
	physx::PxPhysics *GetPhysics() { return m_physics; }
private:
	PhysicsSystem() = default;
	PhysicsSystem(PhysicsSystem&&) = delete;
	PhysicsSystem(const PhysicsSystem&) = delete;
	PhysicsSystem& operator=(PhysicsSystem&&) = delete;
	PhysicsSystem& operator=(const PhysicsSystem&) = delete;

	PhysicsScene* m_physicsScene = nullptr;

	physx::PxFoundation   *m_foundation = nullptr;
	physx::PxPvdTransport *m_pvdTransport = nullptr;
	physx::PxPvd          *m_pvd = nullptr;
	physx::PxPhysics      *m_physics = nullptr;
	physx::PxCooking      *m_cooking = nullptr;
};