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
	void selectEditorMode();
	void cameraUpdate(Input& input, float deltaTime);
	void addObjectInMap();
	void changeGrid(Input& input);
	void selectObjectInMap();
	void updateInSelectMode(Input& input);
	void updateInAddMode(Input& input);
	void removeLastObject();

	// viewport info
	int m_windowWidth = 0;
	int m_windowHeight = 0;
	glm::mat4 m_perspective;
	Camera m_camera;
	bool m_freeLook = false;
	glm::ivec3 m_posCameraFromImGui = glm::ivec3(0);
	glm::ivec3 m_prevPosCameraFromImGui = glm::ivec3(0);

	// editor mode
	EditorMode m_editorMode = EditorMode::Select;
	int m_selectEditorModeFromImGui = 0;



	
	bool m_isCreate = false;

	DrawGrid m_gridDrawer;
	EditorCollectModels m_collectModels;
	EditorCursor m_cursors;

	// editor data
	float m_currentGridHeight = 0.0f;
	float m_currentGridStepHeight = 0.1f;
	float m_currentSizeCell = 1.0f;
	float m_currentSizeMap = 1000.0f;

	EditorMap m_map;
};