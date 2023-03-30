#include "stdafx.h"
#include "BaseClass.h"
#include "EngineDevice.h"
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