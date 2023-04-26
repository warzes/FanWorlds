#pragma once

#include "MeshLibrary.h"

class EditorMap;
class EditorMapObject;

class EditorCollectModels
{
public:
	bool Create(RenderSystem& renderSystem, GraphicsSystem& graphicsSystem);
	void Destroy();

	void DrawPreview(RenderSystem& renderSystem, GraphicsSystem& graphicsSystem, const glm::mat4& proj, const glm::mat4& view, const glm::vec3& centerPos);
	void DrawSelBox(RenderSystem& renderSystem, GraphicsSystem& graphicsSystem, const glm::mat4& proj, const glm::mat4& view, const glm::vec3& pos, const glm::vec3& eulerRot, const glm::vec3& scales);

	void DrawMap(RenderSystem& renderSystem, GraphicsSystem& graphicsSystem, const glm::mat4& proj, const glm::mat4& view, const EditorMap& map);

	void NextMesh();
	void PrevMesh();

	unsigned GetCurrentMeshId() const { return m_currentMesh; }
	std::string GetCurrentMeshName() const { return m_meshLib.meshes[m_currentMesh].meshName; }
	std::string GetFileModelName() const { return m_modelFileName; }
	glm::vec3 GetCurrentWorldPosition() const { return m_currentWorldPos; }
	glm::vec3 GetCurrentScale() const { return m_currentScale; }
	glm::vec3 GetCurrentRotation() const { return m_currentEulerRot; }
	void RotationX(float degrees) { m_currentEulerRot.x += degrees; }
	void RotationY(float degrees) { m_currentEulerRot.y += degrees; }
	void RotationZ(float degrees) { m_currentEulerRot.z += degrees; }
	AABB GetCurrentAABB() const { return m_meshLib.meshes[m_currentMesh].globalAABB; }

private:
	void resetTransform();
	glm::mat4 computeCurrentTransform();
	void computeTransform(const EditorMapObject& object, glm::mat4& world);


	ShaderProgramRef m_shader;
	Uniform m_uniformProjectionMatrix;
	Uniform m_uniformViewMatrix;
	Uniform m_uniformWorldMatrix;

	MeshLibrary m_meshLib;
	std::string m_modelFileName;

	ModelRef m_modelSelBox;
	ShaderProgramRef m_shaderSelBox;
	Uniform m_uniformSelBoxWVPMatrix;

	Texture2DRef m_tempTexture;

	unsigned m_currentMesh = 0;
	glm::vec3 m_currentWorldPos = glm::vec3(0.0f);
	glm::vec3 m_currentScale = glm::vec3(1.0f);
	glm::vec3 m_currentEulerRot = { 0.0f, 0.0f, 0.0f };
};