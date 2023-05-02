#include "stdafx.h"
#include "GameApp.h"
https://www.youtube.com/watch?v=i2M__5rgx74

игрок находится на многоэтажке.
все улицы погружены под воду и открыты только крыши.
можно заниматься крафтом по механике рафта
можно спускаться в здания и возможно немного плавать
аниме стилистика
враги темные


DaggerFall Unity 
	оружие
	текстуры
	спрайты

//-----------------------------------------------------------------------------
bool GameApp::Create()
{
	Print("GameApp Create");

	m_renderUtils.InitFromOther(this);
	if( !m_renderUtils.Create() )
		return false;

	m_cameraUtils.InitFromOther(this);
	m_cameraUtils.Init();
	return true;
}
//-----------------------------------------------------------------------------
void GameApp::Destroy()
{
	m_cameraUtils.Close();
	m_renderUtils.Destroy();
	Print("GameApp Destroy");
}
//-----------------------------------------------------------------------------
void GameApp::Render()
{
	auto& renderSystem = GetRenderSystem();
	auto& graphicsSystem = GetGraphicsSystem();

	renderSystem.ClearFrame();
	m_renderUtils.BeginDraw(m_cameraUtils.GetPerspective(), m_cameraUtils.GetView());
	m_renderUtils.DrawBox(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f));
	m_renderUtils.DrawBox(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f));

	m_renderUtils.DrawBox(glm::vec3(0.0f, -0.55f, 0.0f), glm::vec3(100.0f, 0.1f, 100.0f));


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

	m_cameraUtils.Update(deltaTime);
}
//-----------------------------------------------------------------------------