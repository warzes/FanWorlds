#include "stdafx.h"
#include "PhysicsScene.h"
#include "PhysicsSystem.h"

PhysicsScene::PhysicsScene(PhysicsSystem* physicsSystem) 
	: m_physics(physicsSystem->m_physics)
{
	m_defaultCpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	if( !m_defaultCpuDispatcher )
	{
		physicsSystem->Fatal("Failed to create default PhysX CPU dispatcher.");
		return;
	}

	//m_defaultMaterial = m_physics->createMaterial(0.8f, 0.4f, 0.2f);
	m_defaultMaterial = m_physics->createMaterial(0.8f, 0.8f, 0.25f);
	if( !m_defaultMaterial )
	{
		physicsSystem->Fatal("Failed to create default PhysX material.");
		return;
	}

	physx::PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
	sceneDesc.gravity       = physx::PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = m_defaultCpuDispatcher;
	sceneDesc.filterShader  = physx::PxDefaultSimulationFilterShader;
	m_scene = m_physics->createScene(sceneDesc);
	if( !m_scene )
	{
		physicsSystem->Fatal("Failed to create PhysX scene.");
		return;
	}

	if( physx::PxPvdSceneClient *pvdClient = m_scene->getScenePvdClient() )
	{
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	m_controllerManager = PxCreateControllerManager(*m_scene);
	if( !m_controllerManager )
	{
		physicsSystem->Fatal("Failed to create PhysX controller manager.");
		return;
	}
}

PhysicsScene::~PhysicsScene()
{
	PX_RELEASE(m_controllerManager);
	PX_RELEASE(m_scene);
	PX_RELEASE(m_defaultMaterial);
	PX_RELEASE(m_defaultCpuDispatcher);
}

bool PhysicsScene::Update(const float deltaTime, const float timeScale)
{
	m_timeSinceLastTick += deltaTime * timeScale;

	const float scaledTimestep = m_fixedTimestep * timeScale;
	if( m_timeSinceLastTick < scaledTimestep )
	{
		return false;
	}

	m_scene->simulate(scaledTimestep);
	m_scene->fetchResults(true);
	m_timeSinceLastTick -= scaledTimestep;
	return true;
}

void PhysicsScene::SetSimulationEventCallback(physx::PxSimulationEventCallback* callback)
{
	m_scene->setSimulationEventCallback(callback);
}

physx::PxController* PhysicsScene::CreateController(const glm::vec3& position, float radius, float height, PhysicsLayer queryLayer)
{
	physx::PxCapsuleControllerDesc desc;
	desc.position   = { position.x, position.y, position.z };
	desc.stepOffset = 0.0f;
	desc.material   = m_defaultMaterial;
	// https://nvidia-omniverse.github.io/PhysX/physx/5.1.0/docs/CharacterControllers.html#character-volume
	desc.radius     = radius;
	desc.height     = height;

	physx::PxController* controller = m_controllerManager->createController(desc);

	physx::PxRigidDynamic* rigidbody = controller->getActor();
	PhysicsSetQueryLayer(rigidbody, queryLayer);

	return controller;
}

physx::PxShape* PhysicsScene::CreateShape(const physx::PxGeometry& geometry, bool isExclusive, bool isTrigger)
{
	return m_physics->createShape(
		geometry, 
		*m_defaultMaterial, 
		isExclusive,
		isTrigger 
			? physx::PxShapeFlag::eVISUALIZATION | physx::PxShapeFlag::eTRIGGER_SHAPE 
			: physx::PxShapeFlag::eVISUALIZATION | physx::PxShapeFlag::eSCENE_QUERY_SHAPE | physx::PxShapeFlag::eSIMULATION_SHAPE);
}

physx::PxRigidStatic* PhysicsScene::CreateStatic(const physx::PxTransform& transform)
{
	physx::PxRigidStatic* actor = m_physics->createRigidStatic(transform);
	m_scene->addActor(*actor);
	return actor;
}

physx::PxRigidStatic* PhysicsScene::CreateStatic(
	const physx::PxTransform& transform,
	const physx::PxGeometry& geometry,
	PhysicsLayer queryLayer
)
{
	physx::PxRigidStatic* actor = PxCreateStatic(*m_physics, transform, geometry, *m_defaultMaterial);
	PhysicsSetQueryLayer(actor, queryLayer);
	m_scene->addActor(*actor);
	return actor;
}

physx::PxRigidDynamic* PhysicsScene::CreateDynamic(const physx::PxTransform& transform)
{
	physx::PxRigidDynamic* actor = m_physics->createRigidDynamic(transform);
	m_scene->addActor(*actor);
	return actor;
}

physx::PxRigidDynamic* PhysicsScene::CreateDynamic(
	const physx::PxTransform &transform,
	const physx::PxGeometry  &geometry,
	PhysicsLayer              queryLayer,
	float                     density
)
{
	physx::PxRigidDynamic* actor = PxCreateDynamic(*m_physics, transform, geometry, *m_defaultMaterial, density);
	PhysicsSetQueryLayer(actor, queryLayer);
	m_scene->addActor(*actor);
	return actor;
}

physx::PxRaycastBuffer PhysicsScene::Raycast(
	const physx::PxVec3 &origin,
	const physx::PxVec3 &unitDir,
	const float distance,
	PhysicsLayer layer
) const
{
	physx::PxQueryFilterData queryFilterData;
	queryFilterData.data = PhysicsFilterDataFromLayer(layer);

	physx::PxRaycastBuffer buffer;
	m_scene->raycast(origin, unitDir, distance, buffer, physx::PxHitFlag::eDEFAULT, queryFilterData);
	return buffer;
}

physx::PxSweepBuffer PhysicsScene::Sweep(
	const physx::PxGeometry &geometry,
	const physx::PxTransform &pose,
	const physx::PxVec3 &unitDir,
	float distance,
	PhysicsLayer layer
) const
{
	physx::PxQueryFilterData queryFilterData;
	queryFilterData.data = PhysicsFilterDataFromLayer(layer);

	physx::PxSweepBuffer buffer;
	m_scene->sweep(geometry, pose, unitDir, distance, buffer, physx::PxHitFlag::eDEFAULT, queryFilterData);
	return buffer;
}