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
		m_perspective = glm::perspective(glm::radians(65.0f), GetWindowSizeAspect(), 0.01f, 1000.f);
		renderSystem.SetViewport(m_windowWidth, m_windowHeight);
	}
	renderSystem.ClearFrame();

	const glm::mat4 view = m_camera.GetViewMatrix();
	const glm::mat4 projView = m_perspective * view;

	m_cursors.Draw(renderSystem, projView, m_camera.position, m_currentGridHeight);

	// отрисовка только в режиме выбора
	if( m_editorMode == EditorMode::Select )
	{
		if ( m_isVisibleSelectBox )
			m_collectModels.DrawSelBox(renderSystem, graphicsSystem, m_perspective, view, m_selectBoxPos, m_selectBoxScale);
	}
	// отрисовка только в режиме добавления
	if( !m_freeLook && m_editorMode == EditorMode::Add )
	{
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
	object.Init(m_collectModels);
	m_map.AddObject(object);
}
//-----------------------------------------------------------------------------
void EditorState::selectObjectInMap(Input& input)
{
	const glm::vec2 mousePos = input.GetMousePosition();
	const Ray ray = GetMouseRay(mousePos, input.GetWindowWidth(), input.GetWindowHeight(), m_perspective, m_camera);

	bool find = false;
	RayCollision rayCol;
	for( size_t i = 0; i < m_map.object.size(); i++ )
	{
		auto& obj = m_map.object[i].object;

		AABB temp;
		temp.min = obj.aabb.min + obj.worldPosition;
		temp.max = obj.aabb.max + obj.worldPosition;

		RayCollision boxHitInfo = GetRayCollisionBox(ray, temp);
		if( boxHitInfo.hit && (boxHitInfo.distance < rayCol.distance) ) // ищем ближайший объект
		{
			rayCol = boxHitInfo;
			find = true;
			m_selectBoxPos = obj.worldPosition;
			m_selectBoxScale = obj.aabb.GetDimensions() * 1.1f;
		}
	}
	if( find )
	{
		m_isVisibleSelectBox = true;
	}
}
//-----------------------------------------------------------------------------
void EditorState::updateInSelectMode(Input& input)
{
	if( input.IsMouseButtonPressed(0) )
	{
		// выбор объекта мышкой
		selectObjectInMap(input);
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

	// удаление последнего установленного объекта
	if( input.IsKeyPressed(Input::KEY_BACKSPACE) )
		m_map.RemoveLastObject();
}
//-----------------------------------------------------------------------------