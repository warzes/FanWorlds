#pragma once

class _002OpenGLMotionState : public btDefaultMotionState
{
public:
	_002OpenGLMotionState(const btTransform &transform) : btDefaultMotionState(transform) {}

	void GetWorldTransform(btScalar* transform)
	{
		btTransform trans;
		getWorldTransform(trans);
		trans.getOpenGLMatrix(transform);
	}
};

class _002GameObject
{
public:
	_002GameObject(btCollisionShape* pShape, float mass, const btVector3 &initialPosition = btVector3(0, 0, 0), const btQuaternion &initialRotation = btQuaternion(0, 0, 1, 1));
	~_002GameObject();

	btCollisionShape* GetShape() { return m_pShape; }
	btRigidBody* GetRigidBody() { return m_pBody; }
	btMotionState* GetMotionState() { return m_pMotionState; }

	void GetTransform(btScalar* transform)
	{
		if( m_pMotionState ) m_pMotionState->GetWorldTransform(transform);
	}

private:
	btCollisionShape* m_pShape;
	btRigidBody* m_pBody;
	_002OpenGLMotionState* m_pMotionState;
};

// struct to store our raycasting results
struct _002RayResult
{
	btRigidBody* pBody;
	btVector3 hitPoint;
};

class _002BPRaycasting final : public IApp
{
	bool Create() final;
	void Destroy() final;

	void Render() final;
	void Update(float deltaTime) final;

	_002GameObject* CreateGameObject(
		btCollisionShape* pShape,
		const float &mass,
		const btVector3 &initialPosition = btVector3(0.0f, 0.0f, 0.0f),
		const btQuaternion &initialRotation = btQuaternion(0, 0, 1, 1));

	void ShootBox(const btVector3 &direction);
	void DestroyGameObject(btRigidBody* pBody);

	// picking functions
	btVector3 GetPickingRay(int x, int y);
	bool Raycast(const btVector3 &startPosition, const btVector3 &direction, _002RayResult &output);

private:
	int m_windowWidth = 0;
	int m_windowHeight = 0;

	ShaderProgramRef m_shader;
	Uniform m_uniformProjectionMatrix;
	Uniform m_uniformViewMatrix;
	Uniform m_uniformWorldMatrix;

	glm::mat4 m_perspective;
	ModelRef m_model;
	Texture2DRef m_texture;
	Camera m_camera;

	// core Bullet components
	btBroadphaseInterface* m_pBroadphase = nullptr;
	btCollisionConfiguration* m_pCollisionConfiguration = nullptr;
	btCollisionDispatcher* m_pDispatcher = nullptr;
	btConstraintSolver* m_pSolver = nullptr;
	btDynamicsWorld* m_pWorld = nullptr;

	// an array of our game objects
	std::vector<_002GameObject*> m_objects;
};