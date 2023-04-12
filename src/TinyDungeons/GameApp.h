#pragma once

#include "DrawGrid.h"
#include "GameStateManager.h"
#include "GameStateTest.h"
#include "EditorState.h"

class GameApp final : public IApp
{
	bool Create() final;
	void Destroy() final;

	void Render() final;
	void Update(float deltaTime) final;

private:
	GameStateManager m_gameStateManager;
	GameStateTest m_gameStateTest;
	EditorState m_editorState;
};