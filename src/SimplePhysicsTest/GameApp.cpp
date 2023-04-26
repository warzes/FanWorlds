#include "stdafx.h"
#include "GameApp.h"

//-----------------------------------------------------------------------------
bool GameApp::Create()
{
	Print("GameApp Create");
	
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

	particleArray = new cyclone::Particle[6];
	for( unsigned i = 0; i < 6; i++ )
	{
		world.getParticles().push_back(particleArray + i);
	}

	groundContactGenerator.init(&world.getParticles());
	world.getContactGenerators().push_back(&groundContactGenerator);

	// Create the masses and connections.
	particleArray[0].setPosition(0, 0, 1);
	particleArray[1].setPosition(0, 0, -1);
	particleArray[2].setPosition(-3, 2, 1);
	particleArray[3].setPosition(-3, 2, -1);
	particleArray[4].setPosition(4, 2, 1);
	particleArray[5].setPosition(4, 2, -1);
	for( unsigned i = 0; i < 6; i++ )
	{
		particleArray[i].setMass(BASE_MASS);
		particleArray[i].setVelocity(0, 0, 0);
		particleArray[i].setDamping(0.9f);
		particleArray[i].setAcceleration(cyclone::Vector3::GRAVITY);
		particleArray[i].clearAccumulator();
	}

	rods = new cyclone::ParticleRod[ROD_COUNT];

	rods[0].particle[0] = &particleArray[0];
	rods[0].particle[1] = &particleArray[1];
	rods[0].length = 2;
	rods[1].particle[0] = &particleArray[2];
	rods[1].particle[1] = &particleArray[3];
	rods[1].length = 2;
	rods[2].particle[0] = &particleArray[4];
	rods[2].particle[1] = &particleArray[5];
	rods[2].length = 2;

	rods[3].particle[0] = &particleArray[2];
	rods[3].particle[1] = &particleArray[4];
	rods[3].length = 7;
	rods[4].particle[0] = &particleArray[3];
	rods[4].particle[1] = &particleArray[5];
	rods[4].length = 7;

	rods[5].particle[0] = &particleArray[0];
	rods[5].particle[1] = &particleArray[2];
	rods[5].length = 3.606;
	rods[6].particle[0] = &particleArray[1];
	rods[6].particle[1] = &particleArray[3];
	rods[6].length = 3.606;

	rods[7].particle[0] = &particleArray[0];
	rods[7].particle[1] = &particleArray[4];
	rods[7].length = 4.472;
	rods[8].particle[0] = &particleArray[1];
	rods[8].particle[1] = &particleArray[5];
	rods[8].length = 4.472;

	rods[9].particle[0] = &particleArray[0];
	rods[9].particle[1] = &particleArray[3];
	rods[9].length = 4.123;
	rods[10].particle[0] = &particleArray[2];
	rods[10].particle[1] = &particleArray[5];
	rods[10].length = 7.28;
	rods[11].particle[0] = &particleArray[4];
	rods[11].particle[1] = &particleArray[1];
	rods[11].length = 4.899;
	rods[12].particle[0] = &particleArray[1];
	rods[12].particle[1] = &particleArray[2];
	rods[12].length = 4.123;
	rods[13].particle[0] = &particleArray[3];
	rods[13].particle[1] = &particleArray[4];
	rods[13].length = 7.28;
	rods[14].particle[0] = &particleArray[5];
	rods[14].particle[1] = &particleArray[0];
	rods[14].length = 4.899;

	for( unsigned i = 0; i < ROD_COUNT; i++ )
	{
		world.getContactGenerators().push_back(&rods[i]);
	}

	updateAdditionalMass();


	GetInput().SetMouseLock(true);
	return true;
}
//-----------------------------------------------------------------------------
void GameApp::Destroy()
{
	if( rods ) delete[] rods;
	delete[] particleArray;
	GetInput().SetMouseLock(false);
	m_shader.reset();
	m_model.reset();
	m_texture.reset();
	Print("GameApp Destroy");
}
//-----------------------------------------------------------------------------
void GameApp::Render()
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
	renderSystem.SetUniform(m_uniformProjectionMatrix, m_perspective);
	renderSystem.SetUniform(m_uniformViewMatrix, m_camera.GetViewMatrix());
	renderSystem.SetUniform(m_uniformWorldMatrix, glm::mat4(1.0f));
	renderSystem.SetUniform("DiffuseTexture", 0);
	graphicsSystem.Draw(m_model);

	cyclone::ParticleWorld::Particles &particles = world.getParticles();
	for( auto p = particles.begin(); p != particles.end(); p++ )
	{
		cyclone::Particle *particle = *p;
		const cyclone::Vector3 &pos = particle->getPosition();
		//glTranslatef(pos.x, pos.y, pos.z);
		//glutSolidSphere(0.1f, 20, 10);
	}

	// draw line
	for( unsigned i = 0; i < ROD_COUNT; i++ )
	{
		cyclone::Particle **particles = rods[i].particle;
		const cyclone::Vector3 &p0 = particles[0]->getPosition();
		const cyclone::Vector3 &p1 = particles[1]->getPosition();
		//glVertex3f(p0.x, p0.y, p0.z);
		//glVertex3f(p1.x, p1.y, p1.z);
	}
	
	// draw obj
	//glTranslatef(massDisplayPos.x, massDisplayPos.y + 0.25f, massDisplayPos.z);
	//glutSolidSphere(0.25f, 20, 10);
}
//-----------------------------------------------------------------------------
void GameApp::Update(float deltaTime)
{
	if( GetInput().IsKeyDown(Input::KEY_ESCAPE) )
	{
		BaseClass::ExitRequest();
		return;
	}

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


	// Clear accumulators
	world.startFrame();

	// Find the duration of the last frame in seconds
	//float duration = (float)TimingData::get().lastFrameDuration * 0.001f;
	//if( duration <= 0.0f ) return;
	// Run the simulation
	world.runPhysics(deltaTime);

	updateAdditionalMass();

	if( GetInput().IsKeyDown(Input::KEY_UP) )
	{
		massPos.z += 0.1f;
		if( massPos.z > 1.0f ) massPos.z = 1.0f;
	}
	if( GetInput().IsKeyDown(Input::KEY_DOWN) )
	{
		massPos.z -= 0.1f;
		if( massPos.z < 0.0f ) massPos.z = 0.0f;
	}
	if( GetInput().IsKeyDown(Input::KEY_LEFT) )
	{
		massPos.x -= 0.1f;
		if( massPos.x < 0.0f ) massPos.x = 0.0f;
	}
	if( GetInput().IsKeyDown(Input::KEY_RIGHT) )
	{
		massPos.x += 0.1f;
		if( massPos.x > 1.0f ) massPos.x = 1.0f;
	}
}
//-----------------------------------------------------------------------------
void GameApp::updateAdditionalMass()
{
	for( unsigned i = 2; i < 6; i++ )
	{
		particleArray[i].setMass(BASE_MASS);
	}

	// Find the coordinates of the mass as an index and proportion
	real xp = massPos.x;
	if( xp < 0 ) xp = 0;
	if( xp > 1 ) xp = 1;

	real zp = massPos.z;
	if( zp < 0 ) zp = 0;
	if( zp > 1 ) zp = 1;

	// Calculate where to draw the mass
	massDisplayPos.clear();

	// Add the proportion to the correct masses
	particleArray[2].setMass(BASE_MASS + EXTRA_MASS * (1 - xp) * (1 - zp));
	massDisplayPos.addScaledVector(
		particleArray[2].getPosition(), (1 - xp) * (1 - zp)
	);

	if( xp > 0 )
	{
		particleArray[4].setMass(BASE_MASS + EXTRA_MASS * xp * (1 - zp));
		massDisplayPos.addScaledVector(
			particleArray[4].getPosition(), xp * (1 - zp)
		);

		if( zp > 0 )
		{
			particleArray[5].setMass(BASE_MASS + EXTRA_MASS * xp * zp);
			massDisplayPos.addScaledVector(
				particleArray[5].getPosition(), xp * zp
			);
		}
	}
	if( zp > 0 )
	{
		particleArray[3].setMass(BASE_MASS + EXTRA_MASS * (1 - xp) * zp);
		massDisplayPos.addScaledVector(
			particleArray[3].getPosition(), (1 - xp) * zp
		);
	}
}
//-----------------------------------------------------------------------------