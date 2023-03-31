#include "stdafx.h"
#include "AWorldLight.h"

AWorldLight::AWorldLight(float yaw, float pitch, const glm::vec3 &color, const glm::vec3 &ambientColor)
	: m_color(color),
	m_ambientColor(ambientColor)
{
	GetTransform()
		.RotateY(yaw)
		.RotateX(pitch)
		.ClampPitch();
}

void AWorldLight::Draw(Renderer &renderer)
{
	renderer.SetWorldLight(
		GetTransform().GetForwardVector(),
		m_color,
		m_ambientColor
	);
}