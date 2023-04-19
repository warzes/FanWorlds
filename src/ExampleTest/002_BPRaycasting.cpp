#include "stdafx.h"
#include "002_BPRaycasting.h"
// TODO: доделать
//-----------------------------------------------------------------------------
_002GameObject::_002GameObject(btCollisionShape* pShape, float mass, const btVector3 &initialPosition, const btQuaternion &initialRotation)
{
	// store the shape for later usage
	m_pShape = pShape;

	// create the initial transform
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(initialPosition);
	transform.setRotation(initialRotation);

	// create the motion state from the initial transform
	m_pMotionState = new _002OpenGLMotionState(transform);

	// calculate the local inertia
	btVector3 localInertia(0, 0, 0);

	// objects of infinite mass can't move or rotate
	if( mass != 0.0f )
	{
		pShape->calculateLocalInertia(mass, localInertia);
	}

	// create the rigid body construction info using the mass, motion state and shape
	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, m_pMotionState, pShape, localInertia);

	// create the rigid body
	m_pBody = new btRigidBody(cInfo);
}
//-----------------------------------------------------------------------------
_002GameObject::~_002GameObject()
{
	delete m_pBody;
	delete m_pMotionState;
	delete m_pShape;
}
//-----------------------------------------------------------------------------
bool _002BPRaycasting::Create()
{
	Print("_002BPRaycasting Create");

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
	m_pWorld = new btDiscreteDynamicsWorld(m_pDispatcher, m_pBroadphase, m_pSolver, m_pCollisionConfiguration);

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
void _002BPRaycasting::Destroy()
{
	delete m_pWorld;
	delete m_pSolver;
	delete m_pBroadphase;
	delete m_pDispatcher;
	delete m_pCollisionConfiguration;


	GetInput().SetMouseLock(false);
	m_shader.reset();
	m_model.reset();
	m_texture.reset();

	Print("_002BPRaycasting Destroy");
}
//-----------------------------------------------------------------------------
void _002BPRaycasting::Render()
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
		_002GameObject* pObj = *i;

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
void _002BPRaycasting::Update(float deltaTime)
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

	m_pWorld->stepSimulation(deltaTime);

	glm::vec2 mousePos = GetInput().GetMousePosition();

	if( GetInput().IsKeyPressed(Input::KEY_F) )
	{

		// create a temp object to store the raycast result
		_002RayResult result;
		// perform the raycast
		if( !Raycast({ m_camera.position.x, m_camera.position.y, m_camera.position.z }, GetPickingRay(mousePos.x, mousePos.y), result) )
			return; // return if the test failed
		// destroy the corresponding game object
		DestroyGameObject(result.pBody);
		return;
	}

	if( GetInput().IsMouseButtonPressed(1) )
	{
		// shoot a box
		ShootBox(GetPickingRay(mousePos.x, mousePos.y));
	}
}
//-----------------------------------------------------------------------------
_002GameObject* _002BPRaycasting::CreateGameObject(btCollisionShape* pShape, const float &mass, const btVector3 &initialPosition, const btQuaternion &initialRotation)
{
	_002GameObject* pObject = new _002GameObject(pShape, mass, initialPosition, initialRotation);
	m_objects.push_back(pObject);
	m_pWorld->addRigidBody(pObject->GetRigidBody());
	return pObject;
}
//-----------------------------------------------------------------------------
btVector3 _002BPRaycasting::GetPickingRay(int x, int y)
{
	// calculate the field-of-view
	float tanFov = 1.0f / 0.001f/*m_nearPlane*/;
	float fov = btScalar(2.0) * btAtan(tanFov);

	// get a ray pointing forward from the
	// camera and extend it to the far plane
	btVector3 rayFrom = { m_camera.position.x, m_camera.position.y, m_camera.position.z };
	btVector3 rayForward = { m_camera.GetNormalizedViewVector().x, m_camera.GetNormalizedViewVector().y, m_camera.GetNormalizedViewVector().z};
	rayForward *= 1000.0f/*m_farPlane*/;

	// find the horizontal and vertical vectors
	// relative to the current camera view
	btVector3 ver = btVector3(0, 1, 0);
	btVector3 hor = rayForward.cross(ver);
	hor.normalize();
	ver = hor.cross(rayForward);
	ver.normalize();
	hor *= 2.f * 1000.0f/*m_farPlane*/ * tanFov;
	ver *= 2.f * 1000.0f/*m_farPlane*/ * tanFov;

	// calculate the aspect ratio
	btScalar aspect = GetWindowWidth() / (btScalar)GetWindowHeight();

	// adjust the forward-ray based on
	// the X/Y coordinates that were clicked
	hor *= aspect;
	btVector3 rayToCenter = rayFrom + rayForward;
	btVector3 dHor = hor * 1.f / float(GetWindowWidth());
	btVector3 dVert = ver * 1.f / float(GetWindowHeight());
	btVector3 rayTo = rayToCenter - 0.5f * hor + 0.5f * ver;
	rayTo += btScalar(x) * dHor;
	rayTo -= btScalar(y) * dVert;

	// return the final result
	return rayTo;
}
//-----------------------------------------------------------------------------
void _002BPRaycasting::ShootBox(const btVector3 &direction)
{
	// create a new box object
	_002GameObject* pObject = CreateGameObject(new btBoxShape(btVector3(1, 1, 1)), 1, btVector3(0.4f, 0.f, 0.4f), { m_camera.position.x, m_camera.position.y, m_camera.position.z });

	// calculate the velocity
	btVector3 velocity = direction;
	velocity.normalize();
	velocity *= 25.0f;

	// set the linear velocity of the box
	pObject->GetRigidBody()->setLinearVelocity(velocity);
}
//-----------------------------------------------------------------------------
bool _002BPRaycasting::Raycast(const btVector3 &startPosition, const btVector3 &direction, _002RayResult &output)
{
	if( !m_pWorld )
		return false;

	// get the picking ray from where we clicked
	btVector3 rayTo = direction;
	btVector3 rayFrom = { m_camera.position.x, m_camera.position.y, m_camera.position.z };

	// create our raycast callback object
	btCollisionWorld::ClosestRayResultCallback rayCallback(rayFrom, rayTo);

	// perform the raycast
	m_pWorld->rayTest(rayFrom, rayTo, rayCallback);

	// did we hit something?
	if( rayCallback.hasHit() )
	{
		// if so, get the rigid body we hit
		btRigidBody* pBody = (btRigidBody*)btRigidBody::upcast(rayCallback.m_collisionObject);
		if( !pBody )
			return false;

		// prevent us from picking objects
		// like the ground plane
		if( pBody->isStaticObject() || pBody->isKinematicObject() )
			return false;

		// set the result data
		output.pBody = pBody;
		output.hitPoint = rayCallback.m_hitPointWorld;
		return true;
	}

	// we didn't hit anything
	return false;
}
//-----------------------------------------------------------------------------
void _002BPRaycasting::DestroyGameObject(btRigidBody* pBody)
{
	// we need to search through the objects in order to 
	// find the corresponding iterator (can only erase from 
	// an std::vector by passing an iterator)
	for( auto iter = m_objects.begin(); iter != m_objects.end(); ++iter )
	{
		if( (*iter)->GetRigidBody() == pBody )
		{
			_002GameObject* pObject = *iter;
			// remove the rigid body from the world
			m_pWorld->removeRigidBody(pObject->GetRigidBody());
			// erase the object from the list
			m_objects.erase(iter);
			// delete the object from memory
			delete pObject;
			// done
			return;
		}
	}
}
//-----------------------------------------------------------------------------