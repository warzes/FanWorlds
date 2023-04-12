#pragma once

class GameStateManager;

class IGameState : public BaseClass
{
	friend class GameStateManager;
public:
	virtual ~IGameState() = default;
	void Destroy();

	// OnCreate - �������� ��������
	virtual bool OnCreate() = 0;
	// OnDestroy - �������� ��������
	virtual void OnDestroy() = 0;

	// OnActive - ���� ����� ���-�� �������� ��� �������� � �����, �������� � ����
	virtual void OnActive() = 0;

	virtual void OnUpdate(float deltaTime) = 0;
	virtual void OnFrame() = 0;

private:
	bool create(GameStateManager* manager);


	GameStateManager* m_manager = nullptr;
	bool m_isCreate = false;
};