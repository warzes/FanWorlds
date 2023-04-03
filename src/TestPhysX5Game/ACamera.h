#pragma once

#include "Actor.h"

class ACamera final : public Actor
{
public:
	DEFINE_ACTOR_CLASS(ACamera);
	ACamera(float fov = 90.0f, float near = 0.01f, float far = 1000.0f);

	void Update(float deltaTime) override;

	void Draw(Renderer &renderer) override;

	void SetTargetActor(const Actor *actor)
	{
		m_targetActor = actor;
	}

private:
	float m_fov;
	float m_near;
	float m_far;

	const Actor *m_targetActor = nullptr;
};