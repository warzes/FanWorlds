#pragma once

#include "Actor.h"

class ALightPoint final : public Actor
{
public:
	DEFINE_ACTOR_CLASS(ALightPoint);

	explicit ALightPoint(
		const glm::vec3 &position,
		const glm::vec3 &color = glm::vec3{ 1.0f, 1.0f, 1.0f },
		float range = 20.0f
	);

	void Draw(Renderer &renderer) override;

private:
	glm::vec3 m_color{};
	float m_linear{};
	float m_quadratic{};
};