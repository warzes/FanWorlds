#include "stdafx.h"
#include "GameApp.h"
#if 0 // велосипед без физ движка

glm::vec3 PlayerPos = { 0.0f, 10.0f, 6.0f };
glm::vec3 ColorColide = glm::vec3(1.0f, 0.0f, 1.0f);
glm::vec3 ColorWhite = glm::vec3(1.0f, 1.0f, 1.0f);
bool IsCollide = false;

struct Sphere
{
	glm::vec3 position;
	float radius;

	bool collidesWith(const glm::vec3& point) const
	{
		if( glm::length(point - position) <= radius )
			return true;

		return false;
	}
};


// Функция для нахождения кватерниона, представляющего поворот вокруг оси
glm::quat rotation(float angle, glm::vec3 axis)
{
	glm::quat result;
	float s = std::sin(angle / 2);
	result.w = std::cos(angle / 2);
	result.x = axis.x * s;
	result.y = axis.y * s;
	result.z = axis.z * s;
	return result;
}

// Функция для нахождения кватерниона, представляющего поворот куба вокруг осей
glm::quat rotateCube(float angleX, float angleY, float angleZ)
{
	glm::quat qX = rotation(angleX, { 1, 0, 0 });
	glm::quat qY = rotation(angleY, { 0, 1, 0 });
	glm::quat qZ = rotation(angleZ, { 0, 0, 1 });
	return qX * qY * qZ;
}

// Функция для нахождения расстояния между двумя точками
float distance(glm::vec3 p1, glm::vec3 p2)
{
	float dx = p1.x - p2.x;
	float dy = p1.y - p2.y;
	float dz = p1.z - p2.z;
	return std::sqrt(dx * dx + dy * dy + dz * dz);
}

// Функция для нахождения квадрата расстояния между двумя точками
float distanceSquared(glm::vec3 p1, glm::vec3 p2)
{
	float dx = p1.x - p2.x;
	float dy = p1.y - p2.y;
	float dz = p1.z - p2.z;
	return dx * dx + dy * dy + dz * dz;
}

struct Cube
{
	glm::vec3 position;
	glm::quat orientation;
	float size;
};

// Функция для проверки наличия столкновения между кубом и сферой
bool collision(Cube cube, Sphere sphere)
{
	// Находим координаты сферы в локальной системе координат куба
	glm::quat inverseOrientation = { cube.orientation.w, -cube.orientation.x, -cube.orientation.y, -cube.orientation.z };
	glm::vec3 sphereLocal = {
		(sphere.position.x - cube.position.x) * inverseOrientation.w +
		(sphere.position.y - cube.position.y) * inverseOrientation.z -
		(sphere.position.z - cube.position.z) * inverseOrientation.y,
		(sphere.position.y - cube.position.y) * inverseOrientation.w +
		(sphere.position.z - cube.position.z) * inverseOrientation.x -
		(sphere.position.x - cube.position.x) * inverseOrientation.z,
		(sphere.position.z - cube.position.z) * inverseOrientation.w +
		(sphere.position.x - cube.position.x) * inverseOrientation.y -
		(sphere.position.y - cube.position.y) * inverseOrientation.x
	};

	// Находим ближайшую точку на поверхности куба к сфере
	glm::vec3 closestPoint = sphereLocal;
	if( closestPoint.x > cube.size / 2 ) closestPoint.x = cube.size / 2;
	if( closestPoint.x < -cube.size / 2 ) closestPoint.x = -cube.size / 2;
	if( closestPoint.y > cube.size / 2 ) closestPoint.y = cube.size / 2;
	if( closestPoint.y < -cube.size / 2 ) closestPoint.y = -cube.size / 2;
	if( closestPoint.z > cube.size / 2 ) closestPoint.z = cube.size / 2;
	if( closestPoint.z < -cube.size / 2 ) closestPoint.z = -cube.size / 2;

	// Находим расстояние между ближайшей точкой и сферой
	float distSquared = distanceSquared(closestPoint, sphereLocal);
	float radiusSquared = sphere.radius * sphere.radius;

	// Если расстояние меньше радиуса сферы, то произошло столкновение
	return distSquared <= radiusSquared;
}

// Функция для нахождения точки на поверхности куба, в которой произошло столкновение
glm::vec3 closestPointOnCube(Cube cube, Sphere sphere)
{
	// Находим координаты сферы в локальной системе координат куба
	glm::quat inverseOrientation = { cube.orientation.w, -cube.orientation.x, -cube.orientation.y, -cube.orientation.z };
	glm::vec3 sphereLocal = {
		(sphere.position.x - cube.position.x) * inverseOrientation.w +
		(sphere.position.y - cube.position.y) * inverseOrientation.z -
		(sphere.position.z - cube.position.z) * inverseOrientation.y,
		(sphere.position.y - cube.position.y) * inverseOrientation.w +
		(sphere.position.z - cube.position.z) * inverseOrientation.x -
		(sphere.position.x - cube.position.x) * inverseOrientation.z,
		(sphere.position.z - cube.position.z) * inverseOrientation.w +
		(sphere.position.x - cube.position.x) * inverseOrientation.y -
		(sphere.position.y - cube.position.y) * inverseOrientation.x
	};

	// Находим ближайшую точку на поверхности куба к сфере
	glm::vec3 closestPoint = sphereLocal;
	if( closestPoint.x > cube.size / 2 ) closestPoint.x = cube.size / 2;
	if( closestPoint.x < -cube.size / 2 ) closestPoint.x = -cube.size / 2;
	if( closestPoint.y > cube.size / 2 ) closestPoint.y = cube.size / 2;
	if( closestPoint.y < -cube.size / 2 ) closestPoint.y = -cube.size / 2;
	if( closestPoint.z > cube.size / 2 ) closestPoint.z = cube.size / 2;
	if( closestPoint.z < -cube.size / 2 ) closestPoint.z = -cube.size / 2;

	// Преобразуем координаты ближайшей точки в глобальную систему координат
	glm::vec3 closestPointGlobal = {
		closestPoint.x * cube.orientation.w -
		closestPoint.y * cube.orientation.z +
		closestPoint.z * cube.orientation.y + cube.position.x,
		closestPoint.y * cube.orientation.w -
		closestPoint.z * cube.orientation.x +
		closestPoint.x * cube.orientation.z + cube.position.y,
		closestPoint.z * cube.orientation.w -
		closestPoint.x * cube.orientation.y +
		closestPoint.y * cube.orientation.x + cube.position.z
	};
	return closestPointGlobal;
}

// Функция для вычисления нормали к поверхности куба в заданной точке
glm::vec3 normalAtPoint(Cube cube, glm::vec3 point)
{
	// Находим координаты точки в локальной системе координат куба
	glm::quat inverseOrientation = { cube.orientation.w, -cube.orientation.x, -cube.orientation.y, -cube.orientation.z };
	glm::vec3 pointLocal = {
		(point.x - cube.position.x) * inverseOrientation.w +
		(point.y - cube.position.y) * inverseOrientation.z -
		(point.z - cube.position.z) * inverseOrientation.y,
		(point.y - cube.position.y) * inverseOrientation.w +
		(point.z - cube.position.z) * inverseOrientation.x -
		(point.x - cube.position.x) * inverseOrientation.z,
		(point.z - cube.position.z) * inverseOrientation.w +
		(point.x - cube.position.x) * inverseOrientation.y -
		(point.y - cube.position.y) * inverseOrientation.x
	};

	// Определяем, к какой грани куба ближе точка
	float maxDist = -std::numeric_limits<float>::infinity();
	int maxDir = -1;
	for( int i = 0; i < 6; i++ )
	{
		glm::vec3 faceNormal = { 0, 0, 0 };
		if( i == 0 ) faceNormal = { -1, 0, 0 }; // Грань 1
		if( i == 1 ) faceNormal = { 1, 0, 0 }; // Грань 2
		if( i == 2 ) faceNormal = { 0, -1, 0 }; // Грань 3
		if( i == 3 ) faceNormal = { 0, 1, 0 }; // Грань 4
		if( i == 4 ) faceNormal = { 0, 0, -1 }; // Грань 5
		if( i == 5 ) faceNormal = { 0, 0, 1 }; // Грань 6
		float dist = faceNormal.x * pointLocal.x +
			faceNormal.y * pointLocal.y +
			faceNormal.z * pointLocal.z;
		if( dist > maxDist )
		{
			maxDist = dist;
			maxDir = i;
		}
	}

	// Вычисляем нормаль к грани
	glm::vec3 normal = { 0, 0, 0 };
	if( maxDir == 0 ) normal = { -1, 0, 0 }; // Грань 1
	if( maxDir == 1 ) normal = { 1, 0, 0 }; // Грань 2
	if( maxDir == 2 ) normal = { 0, -1, 0 }; // Грань 3
	if( maxDir == 3 ) normal = { 0, 1, 0 }; // Грань 4
	if( maxDir == 4 ) normal = { 0, 0, -1 }; // Грань 5
	if( maxDir == 5 ) normal = { 0, 0, 1 }; // Грань 6

	// Преобразуем нормаль в глобальную систему координат
	glm::quat orientation = cube.orientation;
	glm::vec3 normalGlobal = {
		normal.x * orientation.w +
		normal.y * orientation.z -
		normal.z * orientation.y,
		normal.y * orientation.w +
		normal.z * orientation.x -
		normal.x * orientation.z,
		normal.z * orientation.w +
		normal.x * orientation.y -
		normal.y * orientation.x
	};
	return normalGlobal;
}

//-----------------------------------------------------------------------------
bool GameApp::Create()
{
	//auto t= glm::rotate(glm::quat(), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	Print("GameApp Create");

	m_renderUtils.InitFromOther(this);
	if( !m_renderUtils.Create() )
		return false;

	m_cameraUtils.InitFromOther(this);
	m_cameraUtils.Init();

	return true;
}
//-----------------------------------------------------------------------------
void GameApp::Destroy()
{
	m_cameraUtils.Close();
	m_renderUtils.Destroy();
	Print("GameApp Destroy");
}
//-----------------------------------------------------------------------------
void GameApp::Render()
{
	auto& renderSystem = GetRenderSystem();
	auto& graphicsSystem = GetGraphicsSystem();

	renderSystem.ClearFrame();
	m_renderUtils.BeginDraw(m_cameraUtils.GetPerspective(), m_cameraUtils.GetView());
	//m_renderUtils.DrawBox(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f));
	m_renderUtils.DrawBox(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f));

	m_renderUtils.DrawBox(glm::vec3(0.0f, -0.55f, 0.0f), glm::vec3(100.0f, 0.1f, 100.0f));

	m_renderUtils.DrawSphere(PlayerPos, glm::vec3(1.0f), IsCollide ? ColorColide : ColorWhite);
	m_renderUtils.DrawAABB(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f));

}
//-----------------------------------------------------------------------------
void GameApp::Update(float deltaTime)
{
	Input& input = GetInput();

	if( input.IsKeyDown(Input::KEY_ESCAPE) )
	{
		BaseClass::ExitRequest();
		return;
	}

	m_cameraUtils.Update(deltaTime);

	glm::vec3 motionDirection = glm::vec3(0.0f);
	// движение
	if( input.IsKeyDown(Input::KEY_I )) motionDirection = motionDirection + glm::vec3(0, 0, 1);
	if( input.IsKeyDown(Input::KEY_K )) motionDirection = motionDirection + glm::vec3(0, 0, -1);
	if( input.IsKeyDown(Input::KEY_J )) motionDirection = motionDirection + glm::vec3(-1, 0, 0);
	if( input.IsKeyDown(Input::KEY_L )) motionDirection = motionDirection + glm::vec3(1, 0, 0);
	if (glm::length(motionDirection) > 0) motionDirection = glm::normalize(motionDirection);

	glm::vec3 newPos = PlayerPos + motionDirection * deltaTime * 2.0f;

	Sphere sp;
	sp.position = newPos;
	sp.radius = 0.5f;
	Cube c;
	c.orientation = rotateCube(0, 0, 0);
	c.position = glm::vec3(1.0f, 0.0f, 0.0f);
	c.size = 1.0f;
	IsCollide = collision(c, sp);

	if( IsCollide )
	{
		glm::vec3 closestPoint = closestPointOnCube(c, sp); // Находим точку на поверхности куба, в которой произошло столкновение
		glm::vec3 normal = normalAtPoint(c, closestPoint); // Находим нормаль к поверхности куба в этой точке

		glm::vec3 slide_velocity = motionDirection - normal * glm::dot(motionDirection, normal);
		if( glm::length(slide_velocity) > 0 ) slide_velocity = glm::normalize(slide_velocity);

		PlayerPos += slide_velocity * deltaTime * 2.0f;
		motionDirection = slide_velocity;
	}
	else
		PlayerPos = newPos;

	// гравитация
	PlayerPos.y -= 9.8f * deltaTime;
	if( PlayerPos.y < 0.0f ) PlayerPos.y = 0.0f;

}
//-----------------------------------------------------------------------------

#else

using namespace reactphysics3d;

// First you need to create the PhysicsCommon object. This is a factory module
// that you can use to create physics world and other objects. It is also responsible
// for logging and memory management
PhysicsCommon physicsCommon;
PhysicsWorld* world = nullptr;
RigidBody* body = nullptr;

TriangleVertexArray* cube1;
TriangleVertexArray* cube2;


glm::vec3 PlayerPos = { 0.0f, 20.0f, 6.0f };
glm::vec3 ColorColide = glm::vec3(1.0f, 0.0f, 1.0f);
glm::vec3 ColorWhite = glm::vec3(1.0f, 1.0f, 1.0f);
bool IsCollide = false;
//-----------------------------------------------------------------------------
bool GameApp::Create()
{
	Print("GameApp Create");

	m_renderUtils.InitFromOther(this);
	if( !m_renderUtils.Create() )
		return false;

	m_cameraUtils.InitFromOther(this);
	m_cameraUtils.Init();

	// Create a physics world
	 world = physicsCommon.createPhysicsWorld();

	 // Create a rigid body in the world
	 Quaternion orientation = Quaternion::identity();
	 Transform transform({ PlayerPos.x, PlayerPos.y, PlayerPos.z }, orientation);
	 body = world->createRigidBody(transform);

	 auto mesh = m_renderUtils.GetModelBox();

	 //std::vector<Vector3> tempVec;
	 //for( int i = 0; i < mesh->subMeshes[0].vertices.size(); i++ )
	 //{
		// auto v = mesh->subMeshes[0].vertices[i];
		// tempVec.push_back({ v.position.x,v.position.y, v.position.z });
	 //}

	 //cube1 = new TriangleVertexArray(tempVec.size(), tempVec.data(), sizeof(Vector3),
		// )


	return true;
}
//-----------------------------------------------------------------------------
void GameApp::Destroy()
{
	m_cameraUtils.Close();
	m_renderUtils.Destroy();
	Print("GameApp Destroy");
}
//-----------------------------------------------------------------------------
void GameApp::Render()
{
	auto& renderSystem = GetRenderSystem();
	auto& graphicsSystem = GetGraphicsSystem();

	renderSystem.ClearFrame();
	m_renderUtils.BeginDraw(m_cameraUtils.GetPerspective(), m_cameraUtils.GetView());
	m_renderUtils.DrawBox(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f));
	m_renderUtils.DrawBox(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f));

	m_renderUtils.DrawBox(glm::vec3(0.0f, -0.55f, 0.0f), glm::vec3(100.0f, 0.1f, 100.0f));

	m_renderUtils.DrawSphere(PlayerPos, glm::vec3(1.0f), IsCollide ? ColorColide : ColorWhite);
	m_renderUtils.DrawAABB(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f));

}
//-----------------------------------------------------------------------------
void GameApp::Update(float deltaTime)
{
	Input& input = GetInput();

	if( input.IsKeyDown(Input::KEY_ESCAPE) )
	{
		BaseClass::ExitRequest();
		return;
	}

	const decimal timeStep = 1.0f / 60.0f;
	world->update(timeStep);

	m_cameraUtils.Update(deltaTime);

	// Get the updated position of the body
	const Transform& transform = body->getTransform();
	const Vector3& position = transform.getPosition();

	// Display the position of the body
	std::cout << "Body Position: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;



	glm::vec3 motionDirection = glm::vec3(0.0f);
	// движение
	if( input.IsKeyDown(Input::KEY_I) ) motionDirection = motionDirection + glm::vec3(0, 0, 1);
	if( input.IsKeyDown(Input::KEY_K) ) motionDirection = motionDirection + glm::vec3(0, 0, -1);
	if( input.IsKeyDown(Input::KEY_J) ) motionDirection = motionDirection + glm::vec3(-1, 0, 0);
	if( input.IsKeyDown(Input::KEY_L) ) motionDirection = motionDirection + glm::vec3(1, 0, 0);
	if( glm::length(motionDirection) > 0 ) motionDirection = glm::normalize(motionDirection);

	glm::vec3 newPos = PlayerPos + motionDirection * deltaTime * 2.0f;

	PlayerPos = newPos;
	// гравитация
	PlayerPos.y -= 9.8f * deltaTime;
	if( PlayerPos.y < 0.0f ) PlayerPos.y = 0.0f;

}
#endif