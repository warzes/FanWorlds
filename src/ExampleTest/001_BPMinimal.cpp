#include "stdafx.h"
#include "001_BPMinimal.h"
//-----------------------------------------------------------------------------
_001GameObject::_001GameObject(btCollisionShape* pShape, float mass, const btVector3 &initialPosition, const btQuaternion &initialRotation)
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
	m_pMotionState = new _001OpenGLMotionState(transform);

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
_001GameObject::~_001GameObject()
{
	delete m_pBody;
	delete m_pMotionState;
	delete m_pShape;
}
//-----------------------------------------------------------------------------
bool _001BPMinimal::Create()
{
	Print("_001BPMinimal Create");

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
	m_pCollisionConfiguration = new btDefaultCollisionConfiguration();
	// create the dispatcher
	m_pDispatcher = new btCollisionDispatcher(m_pCollisionConfiguration);
	// create the broadphase
	m_pBroadphase = new btDbvtBroadphase();
	// create the constraint solver
	m_pSolver = new btSequentialImpulseConstraintSolver();
	// create the world
	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_pDispatcher, m_pBroadphase, m_pSolver, m_pCollisionConfiguration);

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

	GetInput().SetMouseLock(true);
	return true;
}
//-----------------------------------------------------------------------------
void _001BPMinimal::Destroy()
{
	delete m_dynamicsWorld;
	delete m_pSolver;
	delete m_pBroadphase;
	delete m_pDispatcher;
	delete m_pCollisionConfiguration;


	GetInput().SetMouseLock(false);
	m_shader.reset();
	m_model.reset();
	m_texture.reset();

	Print("_001BPMinimal Destroy");
}
//-----------------------------------------------------------------------------
void _001BPMinimal::Render()
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
		_001GameObject* pObj = *i;

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
			world = glm::scale(world, glm::vec3(halfSize.x()*2.0f, halfSize.y() * 2.0f, halfSize.z() * 2.0f));

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
void _001BPMinimal::Update(float deltaTime)
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

		if( GetInput().IsKeyDown(Input::KEY_W) ) m_camera.MoveBy(moveSpeed);
		if( GetInput().IsKeyDown(Input::KEY_S) ) m_camera.MoveBy(-moveSpeed);
		if( GetInput().IsKeyDown(Input::KEY_A) ) m_camera.StrafeBy(moveSpeed);
		if( GetInput().IsKeyDown(Input::KEY_D) ) m_camera.StrafeBy(-moveSpeed);

		glm::vec2 delta = GetInput().GetMouseDeltaPosition();
		if( delta.x != 0.0f )  m_camera.RotateLeftRight(delta.x * mouseSensitivity);
		if( delta.y != 0.0f )  m_camera.RotateUpDown(-delta.y * mouseSensitivity);
	}

	m_dynamicsWorld->stepSimulation(deltaTime);
}
//-----------------------------------------------------------------------------
_001GameObject* _001BPMinimal::CreateGameObject(btCollisionShape* pShape, const float &mass, const btVector3 &initialPosition, const btQuaternion &initialRotation)
{
	_001GameObject* pObject = new _001GameObject(pShape, mass, initialPosition, initialRotation);
	m_objects.push_back(pObject);
	m_dynamicsWorld->addRigidBody(pObject->GetRigidBody());
	return pObject;
}
//-----------------------------------------------------------------------------