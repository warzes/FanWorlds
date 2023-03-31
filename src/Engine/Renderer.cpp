#include "stdafx.h"
#include "Renderer.h"

void Renderer::DebugDrawMarker(const glm::vec3 &position, float size)
{
	static constexpr glm::vec3 X{ 1.0f, 0.0f, 0.0f };
	static constexpr glm::vec3 Y{ 0.0f, 1.0f, 0.0f };
	static constexpr glm::vec3 Z{ 0.0f, 0.0f, 1.0f };
	DebugDrawLine(position, position + X * size, X);
	DebugDrawLine(position, position - X * size, X);
	DebugDrawLine(position, position + Y * size, Y);
	DebugDrawLine(position, position - Y * size, Y);
	DebugDrawLine(position, position + Z * size, Z);
	DebugDrawLine(position, position - Z * size, Z);
}