#include "stdafx.h"
#include "GameApp.h"

bool GameApp::Create()
{
	Print("GameApp Create");
	
	return true;
}

void GameApp::Destroy()
{
	Print("GameApp Destroy");
}

void GameApp::Render()
{
	if( m_windowWidth != GetWindowWidth() || m_windowHeight != GetWindowHeight() )
	{
		m_windowWidth = GetWindowWidth();
		m_windowHeight = GetWindowHeight();
	}
}

void GameApp::Update(float deltaTime)
{
	if( GetInput().IsKeyDown(Input::KEY_ESCAPE) )
	{
		BaseClass::ExitRequest();
		return;
	}	
}