#pragma once

#include "Actor.h"
#include "BaseClass.h"

class APlayerNoClip final : public Actor
{
public:
	DEFINE_ACTOR_CLASS(APlayerNoClip);

	APlayerNoClip() = delete;
	APlayerNoClip(SystemRef& systemRef,
		const glm::vec3 &position = { 0, 0, 5 },
		float yaw = 0.0f,
		float mouseSpeed = 0.001f,
		float flySpeed = 10.0f
	);

	void Update(float deltaTime) override;

	void Draw(Renderer &renderer) override;

private:
	glm::vec3 GetInputDirection();
	void Turn();
	void Move(float deltaTime);

	SystemRef& m_systemRef;

	float m_mouseSpeed;
	float m_flySpeed;
};