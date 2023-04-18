#include "stdafx.h"
#include "EditorState.h"
//-----------------------------------------------------------------------------
void EditorState::selectEditorMode() // переключение режимов редактора
{
	if(      m_selectEditorModeFromImGui == 0 && m_editorMode != EditorMode::Select )
		m_editorMode = EditorMode::Select;
	else if( m_selectEditorModeFromImGui == 1 && m_editorMode != EditorMode::Add )
		m_editorMode = EditorMode::Add;
	else if( m_selectEditorModeFromImGui == 2 && m_editorMode != EditorMode::Voxel )
		m_editorMode = EditorMode::Voxel;
	else if( m_selectEditorModeFromImGui == 3 && m_editorMode != EditorMode::Sector )
		m_editorMode = EditorMode::Sector;
	else if( m_selectEditorModeFromImGui == 4 && m_editorMode != EditorMode::Heightmap )
		m_editorMode = EditorMode::Heightmap;
	else if( GetInput().IsKeyPressed(Input::KEY_TAB) )
	{
		if( m_editorMode == EditorMode::Select )
		{
			m_editorMode = EditorMode::Add;
			m_selectEditorModeFromImGui = 1;
		}
		else
		{
			m_editorMode = EditorMode::Select;
			m_selectEditorModeFromImGui = 0;
		}
		// TODO: другие режимы
	}
}
//-----------------------------------------------------------------------------
bool EditorState::updateImgui()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// телепорт камеры слайдом имгуи
	if( m_posCameraFromImGui != m_prevPosCameraFromImGui ) 
	{
		m_camera.Teleport(m_posCameraFromImGui);
		m_prevPosCameraFromImGui = m_posCameraFromImGui;
	}

	const ImGuiIO& io = ImGui::GetIO();
	if( !io.WantCaptureMouse && !io.WantCaptureKeyboard )
		return false;

	return true;
}
//-----------------------------------------------------------------------------
void EditorState::cameraUpdate(Input& input, float deltaTime)
{
	if( input.IsMouseButtonDown(1) )
	{
		m_freeLook = true;
		input.SetMouseLock(true);
	}
	else
	{
		if( m_freeLook ) input.SetMouseLock(false);
		m_freeLook = false;
	}

	const float mouseSensitivity = 30.0f * deltaTime;
	float moveSpeed = 16.0f * deltaTime;

	if( input.IsKeyDown(Input::KEY_LEFT_SHIFT) ) moveSpeed *= 2.0f;

	if( input.IsKeyDown(Input::KEY_W) ) m_camera.MoveBy(moveSpeed);
	if( input.IsKeyDown(Input::KEY_S) ) m_camera.MoveBy(-moveSpeed);
	if( input.IsKeyDown(Input::KEY_A) ) m_camera.StrafeBy(moveSpeed);
	if( input.IsKeyDown(Input::KEY_D) ) m_camera.StrafeBy(-moveSpeed);
	if( input.IsKeyDown(Input::KEY_Q) ) m_camera.RotateLeftRight(-moveSpeed * 12.0f);
	if( input.IsKeyDown(Input::KEY_E) ) m_camera.RotateLeftRight(moveSpeed * 12.0f);

	if( m_freeLook )
	{
		const glm::vec2 delta = input.GetMouseDeltaPosition();
		if( delta.x != 0.0f )  m_camera.RotateLeftRight(delta.x * mouseSensitivity);
		if( delta.y != 0.0f )  m_camera.RotateUpDown(-delta.y * mouseSensitivity);
	}

	m_prevPosCameraFromImGui = m_posCameraFromImGui = { m_camera.position.x, m_camera.position.y, m_camera.position.z };
}
//-----------------------------------------------------------------------------
void EditorState::changeGrid(Input& input)
{
	if( input.IsKeyDown(Input::KEY_KP_ADD) && input.IsKeyDown(Input::KEY_KP_SUBTRACT) ) // сброс сетки на дефолтную
	{
		m_currentGridHeight = 0.0f;
		m_currentGridStepHeight = 0.1f;
		m_currentSizeCell = 1.0f;
	}
	else if( input.IsKeyPressed(Input::KEY_KP_ADD) )
	{
		if( input.IsKeyDown(Input::KEY_LEFT_CONTROL) )
			m_currentGridStepHeight += 0.1f;
		else if( input.IsKeyDown(Input::KEY_LEFT_SHIFT) )
			m_currentSizeCell += m_currentGridStepHeight;
		else
			m_currentGridHeight += m_currentGridStepHeight;
	}
	else if( input.IsKeyPressed(Input::KEY_KP_SUBTRACT) )
	{
		if( input.IsKeyDown(Input::KEY_LEFT_CONTROL) )
			m_currentGridStepHeight -= 0.1f;
		else if( input.IsKeyDown(Input::KEY_LEFT_SHIFT) )
			m_currentSizeCell -= m_currentGridStepHeight;
		else
			m_currentGridHeight -= m_currentGridStepHeight;
	}
}
//-----------------------------------------------------------------------------