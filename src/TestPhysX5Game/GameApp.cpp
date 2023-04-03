#include "stdafx.h"
#include "GameApp.h"

bool GameApp::Create()
{
	Print("GameApp Create");

	GetFileSystem().Mount("../TestPhysX5Game", "/");

	m_scene = new Scene(*this);
	m_scene->InitFromOther(this);

	createEssentials();

	GetInput().SetMouseLock(true);

	std::vector<EntityDefinition> entities;
	ParseMap(*this, "hello.har", entities);
	LoadEntities(m_scene, entities);

	m_scene->CreateActor<AWorldLight>(
		glm::radians(30.0f),
		glm::radians(45.0f),
		glm::vec3{ 1.0f, 1.0f, 1.0f },
		glm::vec3{ 0.2f, 0.2f, 0.2f }
	);

	return true;
}

void GameApp::Destroy()
{
	Print("GameApp Destroy");

	GetInput().SetMouseLock(false);
	destroyEssentials();
	Texture::ClearCache();
}

void GameApp::Render()
{
	if( m_windowWidth != GetWindowWidth() || m_windowHeight != GetWindowHeight() )
	{
		m_windowWidth = GetWindowWidth();
		m_windowHeight = GetWindowHeight();
		m_renderer->Resize({ m_windowWidth , m_windowHeight });
	}

	m_renderer->BeginDraw();
	m_scene->Draw(*m_renderer);
	m_renderer->EndDraw();
}

void GameApp::Update(float elapsedTimeSeconds)
{
	if( GetInput().IsKeyDown(Input::KEY_ESCAPE) )
	{
		BaseClass::ExitRequest();
		return;
	}

	if( m_physicsScene->Update(elapsedTimeSeconds, m_timeScale) )
	{
		m_scene->FixedUpdate(m_physicsScene->GetFixedTimestep() * m_timeScale);
	}

	if( GetInput().IsKeyDown(GLFW_KEY_TAB) )
	{
		m_timeScale = 0.2f;
	}
	else
	{
		m_timeScale = 1.0f;
	}

	m_renderer->Update(elapsedTimeSeconds * m_timeScale);
	m_scene->Update(elapsedTimeSeconds * m_timeScale);
}

void GameApp::createEssentials()
{
	m_renderer = new DeferredRenderer(GetRenderSystem());
	m_physicsScene = new PhysicsScene(&GetPhysicsSystem());
	GetPhysicsSystem().AddScene(m_physicsScene);
}

void GameApp::destroyEssentials()
{
	delete m_scene;
	delete m_physicsScene;
	delete m_renderer;
}