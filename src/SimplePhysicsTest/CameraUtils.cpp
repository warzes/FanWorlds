#include "stdafx.h"
#include "CameraUtils.h"
//-----------------------------------------------------------------------------
void CameraUtils::Init()
{
	m_camera.Teleport(glm::vec3(0.0f, 1.0f, -4.0f));
	GetInput().SetMouseLock(true);
}
//-----------------------------------------------------------------------------
void CameraUtils::Close()
{
	GetInput().SetMouseLock(false);
}
//-----------------------------------------------------------------------------
void CameraUtils::Update(float deltaTime)
{
	auto& renderSystem = GetRenderSystem();

	if( m_windowWidth != GetWindowWidth() || m_windowHeight != GetWindowHeight() )
	{
		m_windowWidth = GetWindowWidth();
		m_windowHeight = GetWindowHeight();
		m_perspective = glm::perspective(glm::radians(65.0f), GetWindowSizeAspect(), 0.01f, 1000.f);
		renderSystem.SetViewport(m_windowWidth, m_windowHeight);
	}

	const float mouseSensitivity = 10.0f * deltaTime;
	const float moveSpeed = 10.0f * deltaTime;
	const glm::vec3 oldCameraPos = m_camera.position;

	if( GetInput().IsKeyDown(Input::KEY_W) ) m_camera.MoveBy(moveSpeed);
	if( GetInput().IsKeyDown(Input::KEY_S) ) m_camera.MoveBy(-moveSpeed);
	if( GetInput().IsKeyDown(Input::KEY_A) ) m_camera.StrafeBy(moveSpeed);
	if( GetInput().IsKeyDown(Input::KEY_D) ) m_camera.StrafeBy(-moveSpeed);

	glm::vec2 delta = GetInput().GetMouseDeltaPosition();
	if( delta.x != 0.0f )  m_camera.RotateLeftRight(delta.x * mouseSensitivity);
	if( delta.y != 0.0f )  m_camera.RotateUpDown(-delta.y * mouseSensitivity);
}
//-----------------------------------------------------------------------------