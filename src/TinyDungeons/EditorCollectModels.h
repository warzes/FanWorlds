#pragma once

class EditorCollectModels
{
public:
	bool Create(RenderSystem& renderSystem, GraphicsSystem& graphicsSystem);
	void Destroy();

	void DrawPreview(RenderSystem& renderSystem, GraphicsSystem& graphicsSystem, const glm::mat4& proj, const glm::mat4& view, const glm::vec3& cursorPos, float currentGridHeight);

	void NextMesh();
	void PrevMesh();



private:
	ShaderProgramRef m_shader;
	Uniform m_uniformProjectionMatrix;
	Uniform m_uniformViewMatrix;
	Uniform m_uniformWorldMatrix;

	ModelRef m_model;

	unsigned m_currentMesh = 0;
};