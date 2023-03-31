#pragma once

#include "Actor.h"
#include "MapEntity.h"
#include "MapBrushes.h"

namespace physx
{
	class PxRigidStatic;
}

class AWorldSpawn : public Actor
{
public:
	DEFINE_ACTOR_CLASS(AWorldSpawn);
	AWorldSpawn() = delete;
	AWorldSpawn(SystemRef& systemRef, const EntityBrushes &brushes);
	~AWorldSpawn() override;

	void Draw(Renderer &renderer) override;

private:
	SystemRef& m_systemRef;
	Brushes* m_brushes = nullptr;
	physx::PxRigidStatic *m_brushesRigidbody;

	glm::vec3 m_geomMin{};
	glm::vec3 m_geomMax{};
};