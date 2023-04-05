#pragma once

class GameApp final : public IApp
{
	bool Create() final;
	void Destroy() final;

	void Render() final;
	void Update(float deltaTime) final;

private:
	void createEssentials();
	void destroyEssentials();

	Scene* m_scene = nullptr;
	Renderer* m_renderer = nullptr;
	PhysicsScene* m_physicsScene = nullptr;
	float m_timeScale = 1.0f;

	int m_windowWidth = 0;
	int m_windowHeight = 0;
};