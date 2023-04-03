#include "stdafx.h"
#include "AFuncPhys.h"
#include "APlayer.h"

AFuncPhys::AFuncPhys(SystemRef& systemRef, const EntityBrushes &brushes)
{
	m_brushes = new Brushes(systemRef, brushes, PHYSICS_LAYER_0);
	const glm::vec3 &brushesOrigin = m_brushes->GetOrigin();

	GetTransform().SetPosition(brushesOrigin);

	m_brushesRigidbody = systemRef.GetPhysicsSystem().GetPhysicsScene()->CreateDynamic(physx::PxTransform(brushesOrigin.x, brushesOrigin.y, brushesOrigin.z));
	m_brushes->AttachToRigidActor(m_brushesRigidbody);
	physx::PxRigidBodyExt::updateMassAndInertia(*m_brushesRigidbody, 1.0f);

	m_brushesRigidbody->userData = this;
}

AFuncPhys::~AFuncPhys()
{
	delete m_brushes;
	PX_RELEASE(m_brushesRigidbody)
}

void AFuncPhys::FixedUpdate(float fixedDeltaTime)
{
	const physx::PxTransform transform = m_brushesRigidbody->getGlobalPose();
	const glm::vec3 position{ transform.p.x, transform.p.y, transform.p.z };
	const glm::quat rotation{ transform.q.w, transform.q.x, transform.q.y, transform.q.z };

	m_modelMatrix = GetTransform().SetPosition(position).GetTranslationMatrix() * mat4_cast(rotation);
}

void AFuncPhys::Draw(Renderer &renderer)
{
	m_brushes->Draw(renderer, m_modelMatrix);
}

void AFuncPhys::Use(APlayer *player, const physx::PxRaycastHit &hit)
{
	const glm::vec3 &playerPosition = player->GetTransform().GetPosition();
	const physx::PxVec3 &hitPosition = hit.position;
	const physx::PxVec3 force = physx::PxVec3{
	hitPosition.x - playerPosition.x,
	hitPosition.y - playerPosition.y,
	hitPosition.z - playerPosition.z,
	}.getNormalized() * 10.0f;
	physx::PxRigidBodyExt::addForceAtPos(*m_brushesRigidbody, force, hitPosition, physx::PxForceMode::eIMPULSE);
}