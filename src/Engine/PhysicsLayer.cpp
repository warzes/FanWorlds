#include "stdafx.h"
#include "PhysicsLayer.h"
#include "PhysicsUtilities.h"

void PhysicsSetQueryLayer(physx::PxRigidActor *actor, PhysicsLayer layer)
{
	const physx::PxFilterData filterData = PhysicsFilterDataFromLayer(layer);
	PhysicsForEachActorShape(actor, [&filterData](physx::PxShape *shape) { shape->setQueryFilterData(filterData); });
}