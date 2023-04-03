#include "stdafx.h"
#include "APropStatic.h"
#include "MdlLoader.h"
#include "Material.h"

APropStatic::APropStatic(SystemRef& systemRef, const std::string &model, const glm::vec3 &origin)
{
	PhysicsSystem& physicsSystem = systemRef.GetPhysicsSystem();
	PhysicsScene *physicsScene = physicsSystem.GetPhysicsScene();

	MdlLoader loader(systemRef, model);
	m_texture = loader.GetTexture();
	m_mesh = loader.GetMesh();
	std::vector<physx::PxVec3> colliderTriangles = loader.GetColliderTriangles();

	GetTransform().SetPosition(origin);

	m_collider = physicsSystem.CreateTriangleMesh(colliderTriangles.size(), colliderTriangles.data());
	m_rigidbody = physicsScene->CreateStatic(physx::PxTransform(origin.x, origin.y, origin.z), physx::PxTriangleMeshGeometry(m_collider));
	m_rigidbody->userData = this;
}

APropStatic::~APropStatic()
{
	delete m_texture;
	PX_RELEASE(m_rigidbody)
	PX_RELEASE(m_collider)
}

void APropStatic::Draw(Renderer &renderer)
{
	Material material{ nullptr, m_texture };
	renderer.DrawMesh(m_mesh, GetTransform().GetMatrix(), &material);
}