#pragma once

#include "IGameState.h"
#include "EditorCore.h"
#include "DrawGrid.h"

class EditorState final : public IGameState
{
public:
	bool OnCreate();
	void OnDestroy();
	void OnActive();

	void OnUpdate(float deltaTime);
	void OnFrame();

private:
	bool createImgui();
	void destroyImgui();
	bool updateImgui();
	void drawImgui();
	void selectMode();
	void cameraUpdate(float deltaTime);

	int m_windowWidth = 0;
	int m_windowHeight = 0;
	glm::mat4 m_perspective;
	Camera m_camera;
	bool m_freeLook = false;

	EditorMode m_mode = EditorMode::Select;
	
	bool m_isCreate = false;

	DrawGrid m_gridDrawer;

	// editor data
	float m_currentGridHeight = 0.0f;

	// imgui help
	int m_selectMode = 0;
	glm::ivec3 m_camTelData = glm::ivec3(0);
	glm::ivec3 m_oldCamTelData = glm::ivec3(0);
};