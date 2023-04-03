#include "stdafx.h"
#include "Scene.h"

void Scene::Update(const float deltaTime)
{
	// put pending destroy actors from last frame into a temp queue
	std::vector<std::unique_ptr<Actor>> pendingCreationActorsFromLastFrame = std::move(m_pendingCreationActors);
	const std::vector<std::unique_ptr<Actor>> pendingDestroyActorsFromLastFrame = std::move(m_pendingDestroyActors);

	// awake all pending creation actors
	for( auto &actor : pendingCreationActorsFromLastFrame )
	{
		m_actors.push_back(std::move(actor));
	}

	// update all alive actors
	std::vector<std::unique_ptr<Actor>> aliveActors;
	aliveActors.reserve(m_actors.size());
	for( auto &actor : m_actors )
	{
		actor->Update(deltaTime);
		if( actor->IsPendingDestroy() )
		{
			// put pending destroy actors in this frame into m_pendingDestroyActors
			m_pendingDestroyActors.push_back(std::move(actor));
		}
		else
		{
			aliveActors.push_back(std::move(actor));
		}
	}
	m_actors = std::move(aliveActors);

	// pendingDestroyActorsFromLastFrame deconstructs and releases all pending destroy actors
	// destroy pending destroy actors from last frame
	// this strategy will give other actors one frame to cleanup their references
}

void Scene::FixedUpdate(float fixedDeltaTime)
{
	for( auto &actor : m_actors )
	{
		actor->FixedUpdate(fixedDeltaTime);
	}
}

void Scene::Draw(Renderer &renderer)
{
	for( const auto &actor : m_actors )
	{
		actor->Draw(renderer);
	}
}

Actor *Scene::FindFirstActorOfClassImpl(const std::string &className) const
{
	for( const auto &actor : m_actors )
	{
		if( actor->GetActorClassName() == className )
		{
			return actor.get();
		}
	}
	return nullptr;
}