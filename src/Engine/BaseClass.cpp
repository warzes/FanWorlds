#include "stdafx.h"
#include "BaseClass.h"
#include "EngineDevice.h"
//-----------------------------------------------------------------------------
void BaseClass::ExitRequest()
{
	m_engineDevice->m_isExitRequested = true;
}
//-----------------------------------------------------------------------------
void BaseClass::Print(const std::string& msg)
{
	m_engineDevice->m_logSystem.LogPrint(msg);
}
//-----------------------------------------------------------------------------
void BaseClass::Warning(const std::string& msg)
{
	m_engineDevice->m_logSystem.LogWarning(msg);
}
//-----------------------------------------------------------------------------
void BaseClass::Error(const std::string& msg)
{
	m_engineDevice->m_logSystem.LogError(msg);
}
//-----------------------------------------------------------------------------
void BaseClass::Fatal(const std::string& msg)
{
	m_engineDevice->m_logSystem.LogFatal(msg);
}
//-----------------------------------------------------------------------------