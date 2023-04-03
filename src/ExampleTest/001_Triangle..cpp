#include "stdafx.h"
#include "001_Triangle.h"

bool _001Triangle::Create()
{
	Print("001Triangle Create");

	return true;
}

void _001Triangle::Destroy()
{
	Print("001Triangle Destroy");
}

void _001Triangle::Render()
{
	if( m_windowWidth != GetWindowWidth() || m_windowHeight != GetWindowHeight() )
	{
		m_windowWidth = GetWindowWidth();
		m_windowHeight = GetWindowHeight();
	}
}

void _001Triangle::Update(float elapsedTimeSeconds)
{
	if( GetInput().IsKeyDown(Input::KEY_ESCAPE) )
	{
		BaseClass::ExitRequest();
		return;
	}
}