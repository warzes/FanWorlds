#include "stdafx.h"
#include "APlayerNoClip.h"
#include "Input.h"

APlayerNoClip::APlayerNoClip(
	SystemRef& systemRef,
	const glm::vec3 &position, float yaw,
	float mouseSpeed,
	float flySpeed
) 
	: m_systemRef(systemRef)
	, m_mouseSpeed(mouseSpeed)
	, m_flySpeed(flySpeed)
{
	GetTransform().SetPosition(position).RotateY(yaw);
}

void APlayerNoClip::Update(const float deltaTime)
{
	Turn();
	Move(deltaTime);
}

void APlayerNoClip::Draw(Renderer &renderer)
{
	renderer.DrawPointLight(
		GetTransform().GetPosition(),
		{ 1.0f, 1.0f, 1.0f },
		64.0f
	);
}

glm::vec3 APlayerNoClip::GetInputDirection()
{
	Transform &transform = GetTransform();
	return transform.GetHorizontalRightVector() * m_systemRef.GetInput().GetKeyAxis(GLFW_KEY_D, GLFW_KEY_A) +
		glm::vec3{ 0.0f, 1.0f, 0.0f } * m_systemRef.GetInput().GetKeyAxis(GLFW_KEY_E, GLFW_KEY_Q) +
		transform.GetHorizontalForwardVector() * m_systemRef.GetInput().GetKeyAxis(GLFW_KEY_W, GLFW_KEY_S);
}

void APlayerNoClip::Turn()
{
	const glm::vec2 deltaMousePos = m_systemRef.GetInput().GetMouseDeltaPosition();
	GetTransform()
		.RotateX(m_mouseSpeed * -deltaMousePos.y)
		.RotateY(m_mouseSpeed * -deltaMousePos.x)
		.ClampPitch();
}

void APlayerNoClip::Move(float deltaTime)
{
	const glm::vec3 inputDirection = GetInputDirection();
	if( inputDirection.x != 0.0f || inputDirection.y != 0.0f || inputDirection.z != 0.0f )
	{
		GetTransform().Translate(normalize(inputDirection) * (m_flySpeed * deltaTime));
	}
}