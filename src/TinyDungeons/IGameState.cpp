#include "stdafx.h"
#include "IGameState.h"
//-----------------------------------------------------------------------------
void IGameState::Destroy()
{
	OnDestroy();
	m_isCreate = false;
}
//-----------------------------------------------------------------------------
bool IGameState::create(GameStateManager* manager)
{
	m_manager = manager;
	if( !m_isCreate )
	{
		if( !OnCreate() ) return false;
	}
	m_isCreate = true;
	return true;
}
//-----------------------------------------------------------------------------