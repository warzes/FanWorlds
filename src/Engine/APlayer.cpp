#include "stdafx.h"
#include "APlayer.h"
#include "PhysicsSystem.h"
#include "PhysicsScene.h"
#include "Input.h"

APlayer::APlayer(SystemRef& systemRef, const glm::vec3 &position, float yaw, float mouseSpeed) 
	: m_systemRef(systemRef)
	, m_mouseSpeed(mouseSpeed)
{
	m_controller = systemRef.GetPhysicsSystem().GetPhysicsScene()->CreateController(
		{ position.x, position.y, position.z },
		CAPSULE_RADIUS,
		CAPSULE_HEIGHT);
	m_controller->getActor()->userData = this;

	m_respawnPoint = position;
	GetTransform().RotateY(yaw);
	m_previousPosition = position;
}

APlayer::~APlayer()
{
	PX_RELEASE(m_controller)
}

void APlayer::Update(const float deltaTime)
{
	PhysicsScene* physicsScene = m_systemRef.GetPhysicsSystem().GetPhysicsScene();
	Transform& transform = GetTransform();

	// turn
	{
		const glm::vec2 deltaMousePos = m_systemRef.GetInput().GetMouseDeltaPosition();
#ifdef GLM_FORCE_LEFT_HANDED
		transform
			.RotateX(m_mouseSpeed * deltaMousePos.y)
			.RotateY(m_mouseSpeed * deltaMousePos.x)
			.ClampPitch();
#else
		transform
			.RotateX(m_mouseSpeed * -deltaMousePos.y)
			.RotateY(m_mouseSpeed * -deltaMousePos.x)
			.ClampPitch();
#endif
	}

	// move
	{
		m_movementInput =
			transform.GetHorizontalRightVector() * m_systemRef.GetInput().GetKeyAxis(GLFW_KEY_D, GLFW_KEY_A) +
			transform.GetHorizontalForwardVector() * m_systemRef.GetInput().GetKeyAxis(GLFW_KEY_W, GLFW_KEY_S);
		if( m_movementInput.x != 0 || m_movementInput.y != 0 || m_movementInput.z != 0 )
			m_movementInput = glm::normalize(m_movementInput);
	}

	// jump
	{
		bool currSpace = m_systemRef.GetInput().IsKeyDown(GLFW_KEY_SPACE);
		if( !m_prevSpace && currSpace && m_isOnGround )
		{
			m_velocity.y = JUMP_VELOCITY;
		}

		m_prevSpace = currSpace;
	}

	// sync position
	{
		const physx::PxVec3 position = toVec3(m_controller->getPosition());
		const float timeError = glm::min(physicsScene->GetFixedTimestep(), physicsScene->GetFixedUpdateTimeError());
		const glm::vec3 predictedPosition{
		position.x + m_velocity.x * timeError,
		position.y + m_velocity.y * timeError,
		position.z + m_velocity.z * timeError
		};
		GetTransform().SetPosition(predictedPosition + glm::vec3{ 0.0f, CAPSULE_HALF_HEIGHT, 0.0f });
	}

	// raycast for current target
	{
		bool currLmb = m_systemRef.GetInput().IsMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT);

		const glm::vec3 position = GetTransform().GetPosition();
		const glm::vec3 forward = GetTransform().GetForwardVector();

		physx::PxRaycastBuffer buffer = physicsScene->Raycast(
			{ position.x, position.y, position.z },
			{ forward.x, forward.y, forward.z },
			INTERACTION_DISTANCE,
			PHYSICS_LAYER_0
		);

		m_hitActor = nullptr;
		if( buffer.hasBlock )
		{
			const physx::PxRaycastHit &hit = buffer.block;

			m_hitPosition.x = hit.position.x;
			m_hitPosition.y = hit.position.y;
			m_hitPosition.z = hit.position.z;

			if( hit.actor )
			{
				m_hitActor = static_cast<Actor *>(hit.actor->userData);

				if( !m_prevLmb && currLmb && m_hitActor )
				{
					m_hitActor->Use(this, hit);
				}
			}
		}

		m_prevLmb = currLmb;
	}
}

void APlayer::UpdateGround()
{
	// https://nvidia-omniverse.github.io/PhysX/physx/5.1.1/docs/Geometry.html#capsules
	static const physx::PxCapsuleGeometry QUERY_GEOMETRY(CAPSULE_RADIUS, CAPSULE_HALF_HEIGHT);
	static const physx::PxQuat QUERY_ROTATION(physx::PxHalfPi, physx::PxVec3(0.0f, 0.0f, 1.0f));
	static const physx::PxVec3 QUERY_DIRECTION(0.0f, -1.0f, 0.0f);

	physx::PxSweepBuffer buffer = m_systemRef.GetPhysicsSystem().GetPhysicsScene()->Sweep(
		QUERY_GEOMETRY,
		physx::PxTransform(toVec3(m_controller->getPosition()), QUERY_ROTATION),
		QUERY_DIRECTION,
		GROUND_CHECK_DISTANCE,
		PHYSICS_LAYER_0
	);

	// check is touching ground and ground is not too steep
	m_isOnGround = buffer.hasBlock && buffer.block.normal.y >= m_controller->getSlopeLimit();

	// reduce step offset when the player is in air
	m_controller->setStepOffset(m_isOnGround ? GROUND_STEP_OFFSET : AIR_STEP_OFFSET);
}

void APlayer::CalcHorizontalAcceleration(const glm::vec3 &direction, float acceleration, float drag)
{
	m_acceleration.x = direction.x * acceleration - m_velocity.x * drag;
	m_acceleration.z = direction.z * acceleration - m_velocity.z * drag;
}

void APlayer::UpdateAcceleration()
{
	m_acceleration = {};
	float acceleration = m_isOnGround ? GROUND_ACCELERATION : AIR_ACCELERATION;
	float drag = m_isOnGround ? GROUND_DRAG : AIR_DRAG;
	CalcHorizontalAcceleration(m_movementInput, acceleration, drag);
	m_acceleration.y = -GRAVITY;
}

// based on Kalman filter from: https://github.com/Garima13a/Kalman-Filters/blob/master/2.%201D%20Kalman%20Filter%2C%20solution.ipynb
static void KalmanUpdate(glm::vec3 &posPredicted, glm::vec3 &varPredicted, const glm::vec3 &posObserved, const glm::vec3 &varObserved)
{
	const glm::vec3 pos = (varObserved * posPredicted + varPredicted * posObserved) / (varPredicted + varObserved);
	const glm::vec3 var = varPredicted * varObserved / (varPredicted + varObserved);

	posPredicted = pos;
	varPredicted = var;
}

static void KalmanPredict(glm::vec3 &posPredicted, glm::vec3 &varPredicted, const glm::vec3 &deltaObserved, const glm::vec3 &varObserved)
{
	const glm::vec3 mean = posPredicted + deltaObserved;
	const glm::vec3 var = varPredicted + varObserved;

	posPredicted = mean;
	varPredicted = var;
}

void APlayer::FixedUpdate(float fixedDeltaTime)
{
	UpdateGround();

	UpdateAcceleration();

	// move character controller
	m_velocity += m_acceleration * fixedDeltaTime;

	const glm::vec3     displacement = m_velocity * fixedDeltaTime;
	const physx::PxVec3 disp{ displacement.x, displacement.y, displacement.z };
	m_controller->move(disp, 0.0001f, fixedDeltaTime, physx::PxControllerFilters());

	// calc projected velocity
	const physx::PxVec3 pos = toVec3(m_controller->getPosition());
	const glm::vec3     currentPosition{ pos.x, pos.y, pos.z };
	const glm::vec3     deltaPosition = currentPosition - m_previousPosition;
	
	m_velocity = deltaPosition / fixedDeltaTime;
	m_previousPosition = currentPosition;

	// clamp vertical speed (this is a hack)
	m_velocity.y = glm::min(m_velocity.y, JUMP_VELOCITY);

	// check respawn
	if( currentPosition.y < -100.0f )
	{
		// just teleport player back
		m_controller->setPosition({ m_respawnPoint.x, m_respawnPoint.y, m_respawnPoint.z });
		m_velocity = {};
		m_acceleration = {};
		m_previousPosition = m_respawnPoint;
	}
}

void APlayer::Draw(Renderer &renderer)
{
	if( m_hitActor )
	{
		renderer.DebugDrawMarker(m_hitPosition, 0.1f);
	}
}