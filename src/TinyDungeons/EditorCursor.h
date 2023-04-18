#pragma once

#include "Ray.h"

class EditorCursor
{
public:
	bool Create(RenderSystem& renderSystem);
	void Destroy();

	void Update(Input& input, const glm::mat4& proj, const Camera& camera, float gridHeight, float sizeMap, float sizeCell);
	void Draw(RenderSystem& renderSystem, const glm::mat4& projView, const glm::vec3& startPos, float currentGridHeight);

	const glm::vec3& GetPos() const { return m_rayColToGrid.point; }
	const float GetPosX() const { return m_rayColToGrid.point.x; }
	const float GetPosY() const { return m_rayColToGrid.point.y; }
	const float GetPosZ() const { return m_rayColToGrid.point.z; }
	const glm::vec3& GetPosForObject() const { return m_objPosition; }

private:
	ShaderProgramRef m_shaderLine;
	GPUBufferRef m_vboLine;
	VertexArrayRef m_vaoLine;

	RayCollision m_rayColToGrid;
	glm::vec3 m_objPosition = glm::vec3(0.0f);
};