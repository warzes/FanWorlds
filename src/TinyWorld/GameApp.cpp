#include "stdafx.h"
#include "GameApp.h"

//при выходе из данжа игроку дается выбор из двух вариантов с примерным описанием того что ждет (например слева храм с огненной тематикой, справа подводный данж и т.д.)


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