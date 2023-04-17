#pragma once

#include "IGameState.h"
#include "EditorCore.h"
#include "DrawGrid.h"
#include "EditorCollectModels.h"
#include "EditorCursor.h"
#include "EditorWorldObject.h"

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
	void addObjectInMap();
	void modGrid();

	int m_windowWidth = 0;
	int m_windowHeight = 0;
	glm::mat4 m_perspective;
	Camera m_camera;
	bool m_freeLook = false;

	EditorMode m_mode = EditorMode::Select;
	
	bool m_isCreate = false;

	DrawGrid m_gridDrawer;
	EditorCollectModels m_collectModels;
	EditorCursor m_cursors;

	// editor data
	float m_currentGridHeight = 0.0f;
	float m_currentGridStepHeight = 0.1f;
	float m_currentSizeCell = 1.0f;
	float m_currentSizeMap = 1000.0f;
	glm::vec3 m_objPosition = glm::vec3(0.0f);

	EditorMap m_map;

	// imgui help
	int m_selectMode = 0;
	glm::ivec3 m_camTelData = glm::ivec3(0);
	glm::ivec3 m_oldCamTelData = glm::ivec3(0);
};