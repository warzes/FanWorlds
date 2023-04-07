#pragma once

#include "Actor.h"
#include "Texture.h"
#include "MeshBase.h"

namespace physx
{
	class PxTriangleMesh;
	class PxRigidStatic;
}

class APropStatic : public Actor
{
public:
	DEFINE_ACTOR_CLASS(APropStatic);
	APropStatic() = delete;
	APropStatic(SystemRef& systemRef, const std::string &model, const glm::vec3 &origin);
	~APropStatic() override;

	void Draw(Renderer &renderer) override;

private:
	Texture2* m_texture = nullptr;
	MeshBase m_mesh;

	physx::PxTriangleMesh *m_collider = nullptr;
	physx::PxRigidStatic *m_rigidbody = nullptr;
};