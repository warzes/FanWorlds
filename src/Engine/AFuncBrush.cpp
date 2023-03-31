#include "stdafx.h"
#include "AFuncBrush.h"
#include "PhysicsSystem.h"
#include "PhysicsScene.h"

AFuncBrush::AFuncBrush(SystemRef& systemRef, const EntityBrushes &brushes)
{
	m_brushes = new Brushes(systemRef, brushes, PHYSICS_LAYER_0);
	const glm::vec3 &brushesOrigin = m_brushes->GetOrigin();

	GetTransform().SetPosition(brushesOrigin);

	m_brushesRigidbody = systemRef.GetPhysicsSystem().GetPhysicsScene()->CreateStatic(physx::PxTransform(brushesOrigin.x, brushesOrigin.y, brushesOrigin.z));
	m_brushes->AttachToRigidActor(m_brushesRigidbody);

	m_brushesRigidbody->userData = this;
}

AFuncBrush::~AFuncBrush()
{
	delete m_brushes;
	PX_RELEASE(m_brushesRigidbody)
}

void AFuncBrush::FixedUpdate(float fixedDeltaTime)
{
	const physx::PxTransform transform = m_brushesRigidbody->getGlobalPose();
	const glm::vec3 position{ transform.p.x, transform.p.y, transform.p.z };
	const glm::quat rotation{ transform.q.w, transform.q.x, transform.q.y, transform.q.z };

	m_modelMatrix = GetTransform().SetPosition(position).GetTranslationMatrix() * mat4_cast(rotation);
}

void AFuncBrush::Draw(Renderer &renderer)
{
	m_brushes->Draw(renderer, m_modelMatrix);
}

void AFuncBrush::Move(const glm::vec3 &deltaPosition)
{
	physx::PxTransform pose = m_brushesRigidbody->getGlobalPose();
	physx::PxVec3 &position = pose.p;
	position.x += deltaPosition.x;
	position.y += deltaPosition.y;
	position.z += deltaPosition.z;
	m_brushesRigidbody->setGlobalPose(pose);
}