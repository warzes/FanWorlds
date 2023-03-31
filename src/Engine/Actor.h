#pragma once

#include "MoveOnly.h"
#include "Transform.h"
#include "Renderer.h"

namespace physx
{
	struct PxRaycastHit;
}

class APlayer;

#define DEFINE_ACTOR_CLASS(className) \
NO_MOVE_OR_COPY(className) \
static inline const std::string &ClassName = #className; \
const std::string &GetActorClassName() const override { return ClassName; }

class Actor
{
public:
	NO_MOVE_OR_COPY(Actor)

	Actor() = default;
	virtual ~Actor() = default;

	virtual const std::string &GetActorClassName() const = 0;

	template<class T>
	bool IsClass() const
	{
		return T::ClassName == GetActorClassName();
	}

	template<class T>
	T *Cast()
	{
		return IsClass<T>() ? reinterpret_cast<T *>(this) : nullptr;
	}

	virtual void Update(float deltaTime) {}

	virtual void FixedUpdate(float fixedDeltaTime) {}

	virtual void Draw(Renderer &renderer) {}

	virtual void Use(APlayer *player, const physx::PxRaycastHit &hit) {}

	bool IsPendingDestroy() const { return m_pendingDestroy; }

	void Destroy() { m_pendingDestroy = true; }

	const Transform &GetTransform() const { return m_transform; }

	Transform &GetTransform() { return m_transform; }

private:
	bool m_pendingDestroy = false;
	Transform m_transform;
};