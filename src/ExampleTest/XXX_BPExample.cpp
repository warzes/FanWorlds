#include "stdafx.h"
#include "XXX_BPExample.h"
//-----------------------------------------------------------------------------
XXXGameObject::XXXGameObject(btCollisionShape* pShape, float mass, const btVector3 &initialPosition, const btQuaternion &initialRotation)
{
	// store the shape for later usage
	m_pShape = pShape;

	// create the initial transform
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(initialPosition);
	transform.setRotation(initialRotation);

	// create the motion state from the
	// initial transform
	m_pMotionState = new XXXOpenGLMotionState(transform);

	// calculate the local inertia
	btVector3 localInertia(0, 0, 0);

	// objects of infinite mass can't
	// move or rotate
	if( mass != 0.0f )
	{
		pShape->calculateLocalInertia(mass, localInertia);
	}

	// create the rigid body construction
	// info using the mass, motion state
	// and shape
	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, m_pMotionState, pShape, localInertia);

	// create the rigid body
	m_pBody = new btRigidBody(cInfo);
}
//-----------------------------------------------------------------------------
XXXGameObject::~XXXGameObject()
{
	delete m_pBody;
	delete m_pMotionState;
	delete m_pShape;
}
//-----------------------------------------------------------------------------
bool XXXBPExample::Create()
{
	Print("XXXBPExample Create");

	constexpr const char* vertexShaderText = R"(
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 aTexCoord;

uniform mat4 uWorld;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 fNormal;
out vec3 fColor;
out vec2 fTexCoord;

void main()
{
	gl_Position = uProjection * uView * uWorld * vec4(aPos, 1.0);
	fNormal = aNormal;
	fColor = aColor;
	fTexCoord = aTexCoord;
}
)";

	constexpr const char* fragmentShaderText = R"(
#version 330 core

in vec3 fNormal;
in vec3 fColor;
in vec2 fTexCoord;

uniform sampler2D DiffuseTexture;

out vec4 fragColor;

void main()
{
	vec4 textureClr = texture(DiffuseTexture, fTexCoord) * vec4(fColor, 1.0);
	if (textureClr.a < 0.02) discard;
	fragColor = textureClr;
}
)";

	auto& renderSystem = GetRenderSystem();
	auto& graphicsSystem = GetGraphicsSystem();

	m_shader = renderSystem.CreateShaderProgram({ vertexShaderText }, { fragmentShaderText });
	m_uniformProjectionMatrix = renderSystem.GetUniform(m_shader, "uProjection");
	m_uniformViewMatrix = renderSystem.GetUniform(m_shader, "uView");
	m_uniformWorldMatrix = renderSystem.GetUniform(m_shader, "uWorld");

	m_model = graphicsSystem.CreateModel("../ExampleData/models/crate.obj", "../ExampleData/models/");
	m_texture = renderSystem.CreateTexture2D("../ExampleData/textures/container2.png");

	m_camera.Teleport(glm::vec3(0.0f, 0.0f, -5.0f));

	// create the collision configuration
	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	// create the dispatcher
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
	// General purpose, well optimized broadphase, adapts dynamically to the dimensions of the world.
	m_broadphase = new btDbvtBroadphase();
	// create the constraint solver
	m_constraintSolver = new btSequentialImpulseConstraintSolver();
	// create the world
	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_constraintSolver, m_collisionConfiguration);

	// create our scene's physics objects
	//CreateObjects();
	{
		// create a ground plane
		CreateGameObject(new btBoxShape(btVector3(1, 50, 50)), 0, btVector3(0.0f, 0.0f, 0.0f));

		// create our original red box
		CreateGameObject(new btBoxShape(btVector3(1, 1, 1)), 1.0, btVector3(0.0f, 10.0f, 0.0f));

		// create a second box
		CreateGameObject(new btBoxShape(btVector3(1, 1, 1)), 1.0, btVector3(1.25f, 20.0f, 0.0f));
	}


	m_characterShape = new btCapsuleShape(0.5f, 1.8f);
	float mass = 2.0f;
	btVector3 localInertia = btVector3(0.0f, 0.0f, 0.0f); 	// calculate the local inertia
	m_characterShape->calculateLocalInertia(mass, localInertia);

	// create the initial transform
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(0.0f, 10.0f, -2.0f));
	transform.setRotation(btQuaternion(0, 0, 1, 1));
	// create the motion state from the initial transform
	m_pMotionState = new XXXOpenGLMotionState(transform);

	// create the rigid body construction info using the mass, motion state and shape
	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, m_pMotionState, m_characterShape, localInertia);

	m_characterBody = new  btRigidBody(cInfo);

	m_dynamicsWorld->addRigidBody(m_characterBody);

	m_character = new DynamicCharacterController(m_characterBody, m_characterShape);

	GetInput().SetMouseLock(true);
	return true;
}
//-----------------------------------------------------------------------------
void XXXBPExample::Destroy()
{
	delete m_dynamicsWorld;
	delete m_constraintSolver;
	delete m_broadphase;
	delete m_dispatcher;
	delete m_collisionConfiguration;


	GetInput().SetMouseLock(false);
	m_shader.reset();
	m_model.reset();
	m_texture.reset();

	Print("XXXBPExample Destroy");
}
//-----------------------------------------------------------------------------
void XXXBPExample::Render()
{
	auto& renderSystem = GetRenderSystem();
	auto& graphicsSystem = GetGraphicsSystem();

	if( m_windowWidth != GetWindowWidth() || m_windowHeight != GetWindowHeight() )
	{
		m_windowWidth = GetWindowWidth();
		m_windowHeight = GetWindowHeight();
		m_perspective = glm::perspective(glm::radians(45.0f), GetWindowSizeAspect(), 0.01f, 1000.f);
		renderSystem.SetViewport(m_windowWidth, m_windowHeight);
	}


	renderSystem.ClearFrame();
	renderSystem.Bind(m_texture, 0);
	renderSystem.Bind(m_shader);
	renderSystem.SetUniform("DiffuseTexture", 0);
	renderSystem.SetUniform(m_uniformProjectionMatrix, m_perspective);
	renderSystem.SetUniform(m_uniformViewMatrix, m_camera.GetViewMatrix());

	// create an array of 16 floats (representing a 4x4 matrix)
	btScalar transform[16];

	// iterate through all of the objects in our world
	for( auto i = m_objects.begin(); i != m_objects.end(); ++i )
	{
		// get the object from the iterator
		XXXGameObject* pObj = *i;

		// read the transform
		pObj->GetTransform(transform);

		auto pShape = pObj->GetShape();

		// make a different draw call based on the object type
		switch( pShape->getShapeType() )
		{
			// an internal enum used by Bullet for boxes
		case BOX_SHAPE_PROXYTYPE:
		{
			// assume the shape is a box, and typecast it
			const btBoxShape* box = static_cast<const btBoxShape*>(pShape);
			// get the 'halfSize' of the box
			btVector3 halfSize = box->getHalfExtentsWithMargin();

			glm::mat4 world = glm::make_mat4(transform);
			world = glm::scale(world, glm::vec3(halfSize.x() * 2.0f, halfSize.y() * 2.0f, halfSize.z() * 2.0f));

			renderSystem.SetUniform(m_uniformWorldMatrix, world);
			graphicsSystem.Draw(m_model);
			break;
		}
		default:
			// unsupported type
			break;
		}
	}
}
//-----------------------------------------------------------------------------
void XXXBPExample::Update(float deltaTime)
{
	if( GetInput().IsKeyDown(Input::KEY_ESCAPE) )
	{
		BaseClass::ExitRequest();
		return;
	}

	// camera
	{
		const float mouseSensitivity = 10.0f * deltaTime;
		const float moveSpeed = 10.0f * deltaTime;
		const glm::vec3 oldCameraPos = m_camera.position;

		//if( GetInput().IsKeyDown(Input::KEY_W) ) m_camera.MoveBy(moveSpeed);
		//if( GetInput().IsKeyDown(Input::KEY_S) ) m_camera.MoveBy(-moveSpeed);
		//if( GetInput().IsKeyDown(Input::KEY_A) ) m_camera.StrafeBy(moveSpeed);
		//if( GetInput().IsKeyDown(Input::KEY_D) ) m_camera.StrafeBy(-moveSpeed);

		glm::vec2 delta = GetInput().GetMouseDeltaPosition();
		if( delta.x != 0.0f )  m_camera.RotateLeftRight(delta.x * mouseSensitivity);
		if( delta.y != 0.0f )  m_camera.RotateUpDown(-delta.y * mouseSensitivity);
	}

	btVector3 camdir = { 0.0, 0.0, 0.0 };
	if( GetInput().IsKeyDown(Input::KEY_W) )  camdir.setValue(0,0,1 * deltaTime);
	if( GetInput().IsKeyDown(Input::KEY_S) ) camdir.setValue(0, 0, -1* deltaTime);
	if( GetInput().IsKeyDown(Input::KEY_A) ) camdir.setValue(-1 * deltaTime, 0, 0);
	if( GetInput().IsKeyDown(Input::KEY_D) ) camdir.setValue(1 * deltaTime, 0, 0);

	m_characterBody->applyCentralImpulse(camdir);
	//m_character->setMovementDirection(camdir);
	m_character->updateAction(m_dynamicsWorld, deltaTime);
	btScalar transform[16];
	m_pMotionState->GetWorldTransform(transform);
	//btTransform trans;	
	//m_character->getBody()->getMotionState()->getWorldTransform(trans);
	//trans.getOpenGLMatrix(transform);
	m_camera.position = { transform[12], transform[13], transform[14] };
	
	const float fixedTimeStep = 1 / 60.0f;
	m_dynamicsWorld->stepSimulation(deltaTime, 5, fixedTimeStep);
}
//-----------------------------------------------------------------------------
XXXGameObject* XXXBPExample::CreateGameObject(btCollisionShape* pShape, const float &mass, const btVector3 &initialPosition, const btQuaternion &initialRotation)
{
	XXXGameObject* pObject = new XXXGameObject(pShape, mass, initialPosition, initialRotation);
	m_objects.push_back(pObject);
	AddBody(pObject->GetRigidBody());
	return pObject;
}
//-----------------------------------------------------------------------------
void XXXBPExample::AddBody(btRigidBody* body)
{
	m_dynamicsWorld->addRigidBody(body);
}
//-----------------------------------------------------------------------------