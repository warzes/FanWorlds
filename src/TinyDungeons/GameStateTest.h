#pragma once

#include "IGameState.h"
#include "DrawGrid.h"

class GameStateTest final : public IGameState
{
public:
	bool OnCreate();
	void OnDestroy();
	void OnActive();

	void OnUpdate(float deltaTime);
	void OnFrame();

private:
	int m_windowWidth = 0;
	int m_windowHeight = 0;

	ShaderProgramRef m_shader;
	Uniform m_uniformProjectionMatrix;
	Uniform m_uniformViewMatrix;
	Uniform m_uniformWorldMatrix;

	glm::mat4 m_perspective;
	ModelRef m_model;
	Texture2DRef m_textureModel;
	Camera m_camera;

	DrawGrid m_gridDrawer;
	bool m_freeLook = false;

	ShaderProgramRef m_shaderLine;
	GPUBufferRef m_vboLine;
	VertexArrayRef m_vaoLine;

	glm::vec3 m_3dCursorPos = glm::vec3(0.0f);
};