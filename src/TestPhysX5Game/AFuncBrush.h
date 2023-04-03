#pragma once

#include "Actor.h"
#include "MapEntity.h"
#include "MapBrushes.h"

namespace physx
{
	class PxRigidStatic;
}

class AFuncBrush : public Actor
{
public:
	DEFINE_ACTOR_CLASS(AFuncBrush);
	AFuncBrush() = delete;
	AFuncBrush(SystemRef& systemRef, const EntityBrushes &brushes);

	~AFuncBrush() override;

	void FixedUpdate(float fixedDeltaTime) override;

	void Draw(Renderer &renderer) override;

	void Move(const glm::vec3 &deltaPosition);

private:
	Brushes* m_brushes = nullptr;
	physx::PxRigidStatic* m_brushesRigidbody = nullptr;
	glm::mat4 m_modelMatrix{ 1.0f };
};