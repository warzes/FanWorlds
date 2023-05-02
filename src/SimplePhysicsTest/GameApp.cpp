#include "stdafx.h"
#include "GameApp.h"

//-----------------------------------------------------------------------------
bool GameApp::Create()
{
	Print("GameApp Create");

	m_renderUtils.InitFromOther(this);
	if( !m_renderUtils.Create() )
		return false;

	GetInput().SetMouseLock(true);
	return true;
}
//-----------------------------------------------------------------------------
void GameApp::Destroy()
{
	GetInput().SetMouseLock(false);
	m_renderUtils.Destroy();
	Print("GameApp Destroy");
}
//-----------------------------------------------------------------------------
void GameApp::Render()
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
	m_renderUtils.BeginDraw(m_perspective, m_camera.GetViewMatrix());
	m_renderUtils.DrawBox(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f));
	m_renderUtils.DrawBox(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f));
	m_renderUtils.DrawSphere(glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(1.0f));
	m_renderUtils.DrawAABB(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f));

}
//-----------------------------------------------------------------------------
void GameApp::Update(float deltaTime)
{
	if( GetInput().IsKeyDown(Input::KEY_ESCAPE) )
	{
		BaseClass::ExitRequest();
		return;
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