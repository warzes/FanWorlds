#include "stdafx.h"
#include "EditorState.h"
//-----------------------------------------------------------------------------
void EditorState::OnUpdate(float deltaTime)
{
	Input& input = GetInput();

	if( input.IsKeyDown(Input::KEY_ESCAPE) ) // выход из редактора
	{
		BaseClass::ExitRequest();
		return;
	}

	selectEditorMode();               // переключение режимов редактора
	if( updateImgui() ) return;       // выходит, если имгуи перехватил события
	cameraUpdate(input, deltaTime); // движение камеры по миру редактора
	changeGrid(input);              // изменения сетки
	// информация о курсоре в мире редактора
	m_cursors.Update(input, m_perspective, m_camera, m_currentGridHeight, m_currentSizeMap, m_currentSizeCell);

	// update в режимах
	if( m_editorMode == EditorMode::Select )
		updateInSelectMode(input);
	else if( m_editorMode == EditorMode::Add )
		updateInAddMode(input);
}
//-----------------------------------------------------------------------------
void EditorState::OnFrame()
{
	auto& renderSystem = GetRenderSystem();
	auto& graphicsSystem = GetGraphicsSystem();

	if( m_windowWidth != GetWindowWidth() || m_windowHeight != GetWindowHeight() )
	{
		m_windowWidth = GetWindowWidth();
		m_windowHeight = GetWindowHeight();
		m_perspective = glm::perspective(glm::radians(45.0f), GetWindowSizeAspect(), 0.01f, 1000.f);
		renderSystem.SetViewport(m_windowWidth, m_windowHeight);
	}
	renderSystem.ClearFrame();

	const glm::mat4 view = m_camera.GetViewMatrix();
	const glm::mat4 projView = m_perspective * view;

	// отрисовка только в режиме добавления
	if( !m_freeLook && m_editorMode == EditorMode::Add )
	{
		m_cursors.Draw(renderSystem, projView, m_camera.position, m_currentGridHeight);
		m_collectModels.DrawPreview(renderSystem, graphicsSystem, m_perspective, view, m_cursors.GetPosForObject());
	}
	// отрисовка всей карты
	m_collectModels.DrawMap(renderSystem, graphicsSystem, m_perspective, view, m_map);
	// отрисовка сетки
	m_gridDrawer.Draw(renderSystem, projView, { m_currentSizeMap * 0.5f, m_currentGridHeight, m_currentSizeMap * 0.5f }, m_currentSizeCell, m_currentSizeMap);
	// отрисовка гуи
	drawImgui();
}
//-----------------------------------------------------------------------------
void EditorState::addObjectInMap()
{
	EditorMapObject object;
	object.fileNameModel = m_collectModels.GetFileModelName();
	object.nameMeshInFile = m_collectModels.GetCurrentMeshName();
	object.meshIndex = m_collectModels.GetCurrentMeshId();
	object.position = m_collectModels.GetCurrentPosition();
	object.scale = m_collectModels.GetCurrentScale();
	object.rotation = m_collectModels.GetCurrentRotation();

	m_map.AddObject(object);
}
//-----------------------------------------------------------------------------
void EditorState::selectObjectInMap()
{
}
//-----------------------------------------------------------------------------
void EditorState::updateInSelectMode(Input& input)
{
	if( input.IsMouseButtonPressed(0) )
	{
		// выбор объекта мышкой
		selectObjectInMap();
	}
}
//-----------------------------------------------------------------------------
void EditorState::updateInAddMode(Input& input)
{
	const float wheel = input.GetMouseWheelMove();
	if( wheel > 0.0f ) m_collectModels.NextMesh();
	if( wheel < 0.0f ) m_collectModels.PrevMesh();

	if( input.IsMouseButtonPressed(0) )
	{
		// размещение объекта
		addObjectInMap();
	}

	if( input.IsKeyPressed(Input::KEY_BACKSPACE) )
		m_map.RemoveLastObject();
}
//-----------------------------------------------------------------------------
void EditorState::removeLastObject()
{
}
//-----------------------------------------------------------------------------