#include "stdafx.h"
#include "ALightPoint.h"

ALightPoint::ALightPoint(const glm::vec3 &position, const glm::vec3 &color, float range)
	: m_color(color),
	m_linear(PointLightAttenuationLinearFromRange(range)),
	m_quadratic(PointLightAttenuationQuadraticFromRange(range))
{
	GetTransform().SetPosition(position);
}

void ALightPoint::Draw(Renderer &renderer)
{
	const glm::vec3 &position = GetTransform().GetPosition();
	renderer.DrawPointLight(position, m_color, m_linear, m_quadratic);
	renderer.DebugDrawMarker(position, 1.0f);
}