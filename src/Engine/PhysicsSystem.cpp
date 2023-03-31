#include "stdafx.h"
#include "PhysicsSystem.h"
//-----------------------------------------------------------------------------
static physx::PxDefaultAllocator s_Allocator;
static physx::PxDefaultErrorCallback s_ErrorCallback;
//-----------------------------------------------------------------------------
bool PhysicsSystem::Create()
{
	m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, s_Allocator, s_ErrorCallback);
	if (!m_foundation )
	{
		Fatal("Failed to create PhysX foundation.");
		return false;
	}

	m_pvdTransport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);

	m_pvd = PxCreatePvd(*m_foundation);
	if( !m_pvd->connect(*m_pvdTransport, physx::PxPvdInstrumentationFlag::eALL) )
	{
		Warning("Failed to start PVD");
	}

	m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, physx::PxTolerancesScale(), true, m_pvd);
	if( !m_physics )
	{
		Fatal("Failed to create PhysX physics.");
		return false;
	}

	physx::PxCookingParams cookingParams(m_physics->getTolerancesScale());
	m_cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_foundation, cookingParams);
	if( !m_cooking )
	{
		Fatal("Failed to create PhysX cooking.");
		return false;
	}

	Print("PhysX " 
		+ std::to_string(PX_PHYSICS_VERSION_MAJOR) + "."
		+ std::to_string(PX_PHYSICS_VERSION_MINOR) + "."
		+ std::to_string(PX_PHYSICS_VERSION_BUGFIX));

	return true;
}
//-----------------------------------------------------------------------------
void PhysicsSystem::Destroy()
{
	PX_RELEASE(m_cooking);
	PX_RELEASE(m_physics);
	PX_RELEASE(m_pvd);
	PX_RELEASE(m_pvdTransport);
	PX_RELEASE(m_foundation);
}
//-----------------------------------------------------------------------------
physx::PxMaterial* PhysicsSystem::CreateMaterial(physx::PxReal staticFriction, physx::PxReal dynamicFriction, physx::PxReal restitution)
{
	return m_physics->createMaterial(staticFriction, dynamicFriction, restitution);
}
//-----------------------------------------------------------------------------
physx::PxConvexMesh* PhysicsSystem::CreateConvexMesh(physx::PxU32 count, const physx::PxVec3* vertices, physx::PxU16 vertexLimit)
{
	physx::PxConvexMeshDesc desc;
	desc.points.count = count;
	desc.points.stride = sizeof(physx::PxVec3);
	desc.points.data = vertices;
	desc.flags =
		physx::PxConvexFlag::eCOMPUTE_CONVEX |
		physx::PxConvexFlag::eDISABLE_MESH_VALIDATION |
		physx::PxConvexFlag::eFAST_INERTIA_COMPUTATION;
	desc.vertexLimit = vertexLimit;

	physx::PxDefaultMemoryOutputStream buffer;
	if( !m_cooking->cookConvexMesh(desc, buffer) )
	{
		Error("Failed to create convex mesh!");
		return nullptr;
	}

	physx::PxDefaultMemoryInputData input(buffer.getData(), buffer.getSize());
	return m_physics->createConvexMesh(input);
}
//-----------------------------------------------------------------------------
physx::PxTriangleMesh * PhysicsSystem::CreateTriangleMesh(physx::PxU32 count, const physx::PxVec3* vertices)
{
	physx::PxTriangleMeshDesc desc;
	desc.points.count = count;
	desc.points.stride = sizeof(physx::PxVec3);
	desc.points.data = vertices;
	desc.triangles.count = count / 3;
	desc.triangles.stride = 3 * sizeof(physx::PxU32);
	std::vector<physx::PxU32> trianglesData(count);
	for( int i = 0; i < count; i++ )
	{
		trianglesData[i] = i;
	}
	desc.triangles.data = trianglesData.data();

	physx::PxDefaultMemoryOutputStream buffer;
	if( !m_cooking->cookTriangleMesh(desc, buffer) )
	{
		Error("Failed to create triangle mesh!");
		return nullptr;
	}

	physx::PxDefaultMemoryInputData input(buffer.getData(), buffer.getSize());
	return m_physics->createTriangleMesh(input);
}
//-----------------------------------------------------------------------------