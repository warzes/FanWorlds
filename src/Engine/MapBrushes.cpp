#include "stdafx.h"
#include "MapBrushes.h"
#include "PhysicsScene.h"
#include "PhysicsSystem.h"

static glm::vec3 CalculateOrigin(const EntityBrushes &brushes)
{
	float numVertices = 0;
	glm::vec3 sum{ 0.0f };
	for( const auto &collider : brushes.Colliders )
	{
		for( const auto &vertex : collider )
		{
			numVertices += 1.0f;
			sum.x += vertex.x;
			sum.y += vertex.y;
			sum.z += vertex.z;
		}
	}
	return sum / numVertices;
}

Brushes::Brushes(SystemRef& systemRef, const EntityBrushes &brushes, PhysicsLayer layer)
	: m_systemRef(systemRef)
{
	glm::vec3 origin = CalculateOrigin(brushes);
	physx::PxVec3 physxOrigin{ origin.x, origin.y, origin.z };
	m_origin = origin;

	for( const auto &[textureName, vertices] : brushes.TextureToVertices )
	{
		std::vector<VertexBase> localVertices = vertices;
		for( VertexBase &vertex : localVertices )
		{
			vertex.Position -= origin;
		}
		m_facesAndTextures.emplace_back(m_systemRef, localVertices, textureName);
	}

	PhysicsSystem& physicsSystem = m_systemRef.GetPhysicsSystem();
	PhysicsScene *physicsScene = physicsSystem.GetPhysicsScene();

	const physx::PxFilterData filterData = PhysicsFilterDataFromLayer(layer);

	for( const auto &collider : brushes.Colliders )
	{
		std::vector<physx::PxVec3> localCollider = collider;
		for( physx::PxVec3 &vertex : localCollider )
		{
			vertex -= physxOrigin;
		}

		physx::PxConvexMesh *brushCollider = physicsSystem.CreateConvexMesh(localCollider.size(), localCollider.data());
		m_brushColliders.push_back(brushCollider);

		physx::PxShape *brushShape = physicsScene->CreateShape(physx::PxConvexMeshGeometry(brushCollider), true, false);
		brushShape->setQueryFilterData(filterData);
		m_brushShapes.push_back(brushShape);
	}
}

Brushes::~Brushes()
{
	for( physx::PxShape *shape : m_brushShapes )
	{
		PX_RELEASE(shape)
	}

	for( physx::PxConvexMesh *collider : m_brushColliders )
	{
		PX_RELEASE(collider)
	}
}

void Brushes::AttachToRigidActor(physx::PxRigidActor *actor)
{
	for( const auto &shape : m_brushShapes )
	{
		actor->attachShape(*shape);
	}
}

void Brushes::Draw(Renderer &renderer, const glm::mat4 &modelMatrix)
{
	for( auto &facesAndTexture : m_facesAndTextures )
	{
		facesAndTexture.Draw(renderer, modelMatrix);
	}
}