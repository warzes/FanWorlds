#pragma once

class EditorMap;

class EditorCollectModels
{
public:
	bool Create(RenderSystem& renderSystem, GraphicsSystem& graphicsSystem);
	void Destroy();

	void DrawPreview(RenderSystem& renderSystem, GraphicsSystem& graphicsSystem, const glm::mat4& proj, const glm::mat4& view, const glm::vec3& centerPos);
	void DrawSelBox(RenderSystem& renderSystem, GraphicsSystem& graphicsSystem, const glm::mat4& proj, const glm::mat4& view, const glm::vec3& pos, const glm::vec3& scale);

	void DrawMap(RenderSystem& renderSystem, GraphicsSystem& graphicsSystem, const glm::mat4& proj, const glm::mat4& view, const EditorMap& map);


	void NextMesh();
	void PrevMesh();

	unsigned GetCurrentMeshId() const { return m_currentMesh; }
	std::string GetCurrentMeshName() const { return m_model->subMeshes[m_currentMesh].meshName; }
	std::string GetFileModelName() const { return m_modelFileName; }
	glm::vec3 GetCurrentModelPosition() const { return m_currentModelPos; }
	glm::vec3 GetCurrentWorldPosition() const { return m_currentWorldPos; }
	glm::vec3 GetCurrentScale() const { return m_currentScale; }
	glm::quat GetCurrentRotation() const { return m_currentRotation; }
	float GetCurrentRotationY() const { return m_currentRotationY; }
	AABB GetCurrentAABB() const { return m_model->subMeshes[m_currentMesh].GetLocalAABB(); }

private:
	ShaderProgramRef m_shader;
	Uniform m_uniformProjectionMatrix;
	Uniform m_uniformViewMatrix;
	Uniform m_uniformWorldMatrix;

	ModelRef m_model;
	std::string m_modelFileName;

	ModelRef m_modelSelBox;
	ShaderProgramRef m_shaderSelBox;
	Uniform m_uniformSelBoxWVPMatrix;

	unsigned m_currentMesh = 0;
	glm::vec3 m_currentModelPos = glm::vec3(0.0f);
	glm::vec3 m_currentWorldPos = glm::vec3(0.0f);
	glm::vec3 m_currentScale = glm::vec3(1.0f);
	glm::quat m_currentRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	float m_currentRotationY = 45.0f; // TODO: в будущем удалить
};