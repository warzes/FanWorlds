#include "stdafx.h"
#include "BaseClass.h"
#include "EngineDevice.h"
//-----------------------------------------------------------------------------
void BaseClass::InitFromOther(BaseClass* other)
{
	m_engineDevice = other->m_engineDevice;
}
//-----------------------------------------------------------------------------
void BaseClass::ExitRequest()
{
	assert(m_engineDevice);
	m_engineDevice->m_isExitRequested = true;
}
//-----------------------------------------------------------------------------
void BaseClass::Print(const std::string& msg)
{
	assert(m_engineDevice);
	m_engineDevice->m_logSystem.LogPrint(msg);
}
//-----------------------------------------------------------------------------
void BaseClass::Warning(const std::string& msg)
{
	assert(m_engineDevice);
	m_engineDevice->m_logSystem.LogWarning(msg);
}
//-----------------------------------------------------------------------------
void BaseClass::Error(const std::string& msg)
{
	assert(m_engineDevice);
	m_engineDevice->m_logSystem.LogError(msg);
}
//-----------------------------------------------------------------------------
void BaseClass::Fatal(const std::string& msg)
{
	assert(m_engineDevice);
	m_engineDevice->m_logSystem.LogFatal(msg);
}
//-----------------------------------------------------------------------------
FileSystem& BaseClass::GetFileSystem()
{
	assert(m_engineDevice);
	return m_engineDevice->m_fileSystem;
}
//-----------------------------------------------------------------------------
const FileSystem& BaseClass::GetFileSystem() const
{
	assert(m_engineDevice);
	return m_engineDevice->m_fileSystem;
}
//-----------------------------------------------------------------------------
Window& BaseClass::GetWindow()
{
	assert(m_engineDevice);
	return m_engineDevice->m_window;
}
//-----------------------------------------------------------------------------
const Window& BaseClass::GetWindow() const
{
	assert(m_engineDevice);
	return m_engineDevice->m_window;
}
//-----------------------------------------------------------------------------
Input& BaseClass::GetInput()
{
	assert(m_engineDevice);
	return m_engineDevice->m_input;
}
//-----------------------------------------------------------------------------
const Input& BaseClass::GetInput() const
{
	assert(m_engineDevice);
	return m_engineDevice->m_input;
}
//-----------------------------------------------------------------------------
RenderSystem& BaseClass::GetRenderSystem()
{
	assert(m_engineDevice);
	return m_engineDevice->m_renderSystem;
}
//-----------------------------------------------------------------------------
const RenderSystem& BaseClass::GetRenderSystem() const
{
	assert(m_engineDevice);
	return m_engineDevice->m_renderSystem;
}
//-----------------------------------------------------------------------------
GraphicsSystem& BaseClass::GetGraphicsSystem()
{
	assert(m_engineDevice);
	return m_engineDevice->m_graphicsSystem;
}
//-----------------------------------------------------------------------------
const GraphicsSystem& BaseClass::GetGraphicsSystem() const
{
	assert(m_engineDevice);
	return m_engineDevice->m_graphicsSystem;
}
//-----------------------------------------------------------------------------
WorldManager& BaseClass::GetWorldManager()
{
	assert(m_engineDevice);
	return m_engineDevice->m_worldManager;
}
//-----------------------------------------------------------------------------
const WorldManager& BaseClass::GetWorldManager() const
{
	assert(m_engineDevice);
	return m_engineDevice->m_worldManager;
}
//-----------------------------------------------------------------------------
PhysicsSystem& BaseClass::GetPhysicsSystem()
{
	assert(m_engineDevice);
	return m_engineDevice->m_physicsSystem;
}
//-----------------------------------------------------------------------------
const PhysicsSystem& BaseClass::GetPhysicsSystem() const
{
	assert(m_engineDevice);
	return m_engineDevice->m_physicsSystem;
}
//-----------------------------------------------------------------------------
int BaseClass::GetWindowWidth() const
{
	assert(m_engineDevice);
	return m_engineDevice->m_window.GetWidth();
}
//-----------------------------------------------------------------------------
int BaseClass::GetWindowHeight() const
{
	assert(m_engineDevice);
	return m_engineDevice->m_window.GetHeight();
}
//-----------------------------------------------------------------------------
float BaseClass::GetWindowSizeAspect() const
{
	assert(m_engineDevice);
	const float width = static_cast<float>(GetWindowWidth());
	const float height = static_cast<float>(GetWindowHeight());

	return width / (height > 0.0f ? height : 1.0f);
}
//-----------------------------------------------------------------------------