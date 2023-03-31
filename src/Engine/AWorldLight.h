#pragma once

#include "Actor.h"

class AWorldLight final : public Actor
{
public:
	DEFINE_ACTOR_CLASS(AWorldLight);

	explicit AWorldLight(float yaw, float pitch, const glm::vec3 &color, const glm::vec3 &ambientColor);

	void Draw(Renderer &renderer) override;

private:
	glm::vec3 m_color;
	glm::vec3 m_ambientColor;
};