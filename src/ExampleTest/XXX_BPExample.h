#pragma once

#pragma once

class XXXOpenGLMotionState : public btDefaultMotionState
{
public:
	XXXOpenGLMotionState(const btTransform &transform) : btDefaultMotionState(transform) {}

	void GetWorldTransform(btScalar* transform)
	{
		btTransform trans;
		getWorldTransform(trans);
		trans.getOpenGLMatrix(transform);
	}
};

class XXXGameObject
{
public:
	XXXGameObject(btCollisionShape* pShape, float mass, const btVector3 &initialPosition = btVector3(0, 0, 0), const btQuaternion &initialRotation = btQuaternion(0, 0, 1, 1));
	~XXXGameObject();

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
	XXXOpenGLMotionState* m_pMotionState;
};

class XXXBPExample final : public IApp
{
	bool Create() final;
	void Destroy() final;

	void Render() final;
	void Update(float deltaTime) final;

	XXXGameObject* CreateGameObject(
		btCollisionShape* pShape,
		const float &mass,
		const btVector3 &initialPosition = btVector3(0.0f, 0.0f, 0.0f),
		const btQuaternion &initialRotation = btQuaternion(0, 0, 1, 1));

	void AddBody(btRigidBody* body);

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
	btBroadphaseInterface* m_broadphase = nullptr; //  Broadphase collision detection provides acceleration structure to quickly reject pairs of objects based on axis aligned bounding box(AABB) overlap.
	btCollisionConfiguration* m_collisionConfiguration = nullptr; // Allows to configure Bullet collision detection stack allocator size, default collision algorithms and persistent manifold pool size
	btCollisionDispatcher* m_dispatcher = nullptr; // A collision dispatcher iterates over each pair, searches for a matching collision algorithm based on the types of objects involved and executes the collision algorithm computing contact points.
	btConstraintSolver* m_constraintSolver = nullptr;

	btDynamicsWorld* m_dynamicsWorld = nullptr; // Stores all btCollisionObjects and provides an interface to perform queries.

	// an array of our game objects
	std::vector<XXXGameObject*> m_objects;
};