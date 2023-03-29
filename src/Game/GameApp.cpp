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
}

void GameApp::Update(float elapsedTimeSeconds)
{
	BaseClass::ExitRequest();
}