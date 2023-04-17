#pragma once

class EditorMap;

class EditorCollectModels
{
public:
	bool Create(RenderSystem& renderSystem, GraphicsSystem& graphicsSystem);
	void Destroy();

	void DrawPreview(RenderSystem& renderSystem, GraphicsSystem& graphicsSystem, const glm::mat4& proj, const glm::mat4& view, const glm::vec3& centerPos);

	void Draw(RenderSystem& renderSystem, GraphicsSystem& graphicsSystem, const glm::mat4& proj, const glm::mat4& view, const EditorMap& map);


	void NextMesh();
	void PrevMesh();

	unsigned GetCurrentMeshId() const { return m_currentMesh; }
	std::string GetCurrentMeshName() const { return m_model->subMeshes[m_currentMesh].meshName; }
	std::string GetFileModelName() const { return m_modelFileName; }
	glm::vec3 GetCurrentPosition() const { return m_currentPos; }
	glm::vec3 GetCurrentScale() const { return m_currentScale; }
	glm::quat GetCurrentRotation() const { return m_currentRotation; }

private:
	ShaderProgramRef m_shader;
	Uniform m_uniformProjectionMatrix;
	Uniform m_uniformViewMatrix;
	Uniform m_uniformWorldMatrix;

	ModelRef m_model;
	std::string m_modelFileName;

	unsigned m_currentMesh = 0;
	glm::vec3 m_currentPos = glm::vec3(0.0f);
	glm::vec3 m_currentScale = glm::vec3(1.0f);
	glm::quat m_currentRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
};