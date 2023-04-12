#pragma once

#include "IGameState.h"

class GameStateManager : public BaseClass
{
public:
	bool SetActiveState(IGameState* state);

	void Update(float deltaTime);
	void Frame();

private:
	IGameState* m_currentState = nullptr;
};