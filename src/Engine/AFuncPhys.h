#pragma once

#include "Actor.h"
#include "MapEntity.h"
#include "MapBrushes.h"
#include "BaseClass.h"

namespace physx
{
	class PxRigidDynamic;
}

class AFuncPhys : public Actor
{
public:
	DEFINE_ACTOR_CLASS(AFuncPhys);
	AFuncPhys() = delete;
	AFuncPhys(SystemRef& systemRef, const EntityBrushes &brushes);
	~AFuncPhys() override;

	void FixedUpdate(float fixedDeltaTime) override;

	void Draw(Renderer &renderer) override;

	void Use(APlayer *player, const physx::PxRaycastHit &hit) override;

private:
	Brushes* m_brushes = nullptr;
	physx::PxRigidDynamic* m_brushesRigidbody = nullptr;

	glm::mat4 m_modelMatrix{ 1.0f };
};