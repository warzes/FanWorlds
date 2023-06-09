#pragma once

class _001OpenGLMotionState : public btDefaultMotionState
{
public:
	_001OpenGLMotionState(const btTransform &transform) : btDefaultMotionState(transform) {}

	void GetWorldTransform(btScalar* transform)
	{
		btTransform trans;
		getWorldTransform(trans);
		trans.getOpenGLMatrix(transform);
	}
};

class _001GameObject
{
public:
	_001GameObject(btCollisionShape* pShape, float mass, const btVector3 &initialPosition = btVector3(0, 0, 0), const btQuaternion &initialRotation = btQuaternion(0, 0, 1, 1));
	~_001GameObject();

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
	_001OpenGLMotionState* m_pMotionState;
};

class _001BPMinimal final : public IApp
{
	bool Create() final;
	void Destroy() final;

	void Render() final;
	void Update(float deltaTime) final;

	_001GameObject* CreateGameObject(
		btCollisionShape* pShape,
		const float &mass,
		const btVector3 &initialPosition = btVector3(0.0f, 0.0f, 0.0f),
		const btQuaternion &initialRotation = btQuaternion(0, 0, 1, 1));

private:
	int m_windowWidth = 0;
	int m_windowHeight = 0;

	ShaderProgramRef m_shader;
	Uniform m_uniformProjectionMatrix;
	Uniform m_uniformViewMatrix;
	Uniform m_uniformWorldMatrix;

	glm::mat4 m_perspective;
	ModelRef m_modelBox;
	Texture2DRef m_texture;
	Camera m_camera;

	// core Bullet components
	btBroadphaseInterface* m_pBroadphase = nullptr;
	btCollisionConfiguration* m_pCollisionConfiguration = nullptr;
	btCollisionDispatcher* m_pDispatcher = nullptr;
	btConstraintSolver* m_pSolver = nullptr;
	btDynamicsWorld* m_dynamicsWorld = nullptr;

	// an array of our game objects
	std::vector<_001GameObject*> m_objects;
};