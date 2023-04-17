#pragma once

class EditorCursor
{
public:
	bool Create(RenderSystem& renderSystem);
	void Destroy();

	void Draw(RenderSystem& renderSystem, Input& input, const glm::mat4& proj, const Camera& camera, float currentGridHeight);

	const glm::vec3& GetPos() const { return m_pos; }
	const float GetPosX() const { return m_pos.x; }
	const float GetPosY() const { return m_pos.y; }
	const float GetPosZ() const { return m_pos.z; }

private:
	ShaderProgramRef m_shaderLine;
	GPUBufferRef m_vboLine;
	VertexArrayRef m_vaoLine;

	glm::vec3 m_pos;
};