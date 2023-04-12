#include "stdafx.h"
#include "GameStateManager.h"

bool GameStateManager::SetActiveState(IGameState* state)
{
	if( !state->create(this) )
		return false;

	state->OnActive();
	m_currentState = state;
	return true;
}

void GameStateManager::Update(float deltaTime)
{
	if( m_currentState ) m_currentState->OnUpdate(deltaTime);
}

void GameStateManager::Frame()
{
	if( m_currentState ) m_currentState->OnFrame();
}