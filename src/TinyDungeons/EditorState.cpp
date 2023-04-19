#include "stdafx.h"
#include "EditorState.h"

в рпг ин боксе посмотреть что может редактор

план:
- сделать CollectMeshes
- вращение объекта
- отображение свойств объекта в инспекторе
- в инспекторе показывать координаты с возможностью редактировтаь
- возможно движение выбранного объекта мышкой
- сохранение карты
- новая карта
- открытие карты
- возможность накладывать текстуру
- окно выбора меша
- окно выбора текстуры





карта - в рпг mz есть первый мир - можно ее.или из Лрпг


https ://sketchfab.com/3d-models/tilable-pixel-asset-pack-ca56ca86794b47c29e1592172b10d50a
вот тут посмотреть как данж рисует - https ://www.youtube.com/watch?v=tAr8zYDdI8s&t=521s


	возможность накладывать текстуры на объекты(только для простых форм ? )
	ладншафт мира такой - тайл, на него натягивается текстура(для первой текстуры из рпг мейкера собранные в тайледе - где нужны стыки текстур)

	как сделать мир игры

	есть глобальная карта - игрок путешествует с видом сверху
	есть локальные карты типа данжей.там игрок от первого лица

	на глобальной карте игрок в любой точке может перейти на локалку - тогда рандомно сгенерится карта под эту область.можно собирать всякое.
	иногда вместо рандома может появится вручную сделанная карта с событием.

	так что в редакторе придется сделать сохранение карты под файлом
	подгрузит этот тайлсет для данжей - https://kaylousberg.itch.io/kaykit-dungeon-remastered

деградация - в мире постепенно все становится хуже и хуже.
стилистика гг - аниме героиня.
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

	m_map.Update();
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

	m_isVisibleSelectBox = false;
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
			m_isVisibleSelectBox = true;
			m_selectBoxPos = obj.worldPosition;
			m_selectBoxScale = obj.aabb.GetDimensions() * 1.1f;
			m_selectObjectId = i;
		}
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
	// удаление выбранного объекта
	if( input.IsKeyPressed(Input::KEY_DELETE) && m_isVisibleSelectBox )
	{
		m_map.RemoveObject(m_selectObjectId);
		resetSelect();
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
	{
		m_map.RemoveLastObject();
		resetSelect();
	}
}
//-----------------------------------------------------------------------------
void EditorState::resetSelect()
{
	m_isVisibleSelectBox = false;
	m_selectObjectId = std::numeric_limits<unsigned>::max();
}
//-----------------------------------------------------------------------------