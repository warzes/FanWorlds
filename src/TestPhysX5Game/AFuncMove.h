#pragma once

#include "AFuncBrush.h"

class AFuncMove : public AFuncBrush
{
public:
	DEFINE_ACTOR_CLASS(AFuncMove);
	AFuncMove() = delete;
	AFuncMove(SystemRef& systemRef, const EntityBrushes &brushes, const glm::vec3 &moveSpeed, float moveTime)
		: AFuncBrush(systemRef, brushes)
		, m_moveSpeed(moveSpeed)
		, m_moveTime(moveTime)
	{
	}

	void FixedUpdate(float fixedDeltaTime) override;

	void Use(APlayer *player, const physx::PxRaycastHit &hit) override;

	void Open();

	void Close();

	void Toggle();

private:
	enum class State
	{
		Close, MovingOpen, Open, MovingClose
	};

	State m_state = State::Close;
	float m_remainingTime = 0.0f;

	glm::vec3 m_moveSpeed{ 0.0f, 0.0f, 0.0f };
	float m_moveTime = 0.0f;
};