#pragma once

#include "MapEntity.h"
#include "FacesAndTexture.h"
#include "PhysicsLayer.h"
#include "BaseClass.h"

namespace physx
{
	class PxConvexMesh;
	class PxShape;
	class PxRigidActor;
}

class Brushes
{
public:
	MOVE_ONLY(Brushes);
	Brushes() = delete;
	Brushes(SystemRef& systemRef, const EntityBrushes &brushes, PhysicsLayer layer = PHYSICS_LAYER_0);

	~Brushes();

	void AttachToRigidActor(physx::PxRigidActor *actor);

	void Draw(Renderer &renderer, const glm::mat4 &modelMatrix);

	[[nodiscard]] const glm::vec3 &GetOrigin() const { return m_origin; }

private:
	SystemRef& m_systemRef;
	MoveOnly<glm::vec3> m_origin;

	std::vector<FacesAndTexture> m_facesAndTextures;
	std::vector<physx::PxConvexMesh *> m_brushColliders;
	std::vector<physx::PxShape *> m_brushShapes;
};