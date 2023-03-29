#include "stdafx.h"
#include "LogSystem.h"
//-----------------------------------------------------------------------------
LogSystem::~LogSystem()
{
	destroy();
}
//-----------------------------------------------------------------------------
void LogSystem::LogPrint(const std::string& msg)
{
	puts(msg.c_str());
#if defined(_WIN32)
	if( m_logFile ) fputs((msg + "\n").c_str(), m_logFile);
#endif
}
//-----------------------------------------------------------------------------
void LogSystem::LogWarning(const std::string& msg)
{
	LogPrint("Warning: " + msg);
}
//-----------------------------------------------------------------------------
void LogSystem::LogError(const std::string& msg)
{
	LogPrint("Error: " + msg);
}
//-----------------------------------------------------------------------------
void LogSystem::LogFatal(const std::string& msg)
{
	BaseClass::ExitRequest();
	LogError(msg);
}
//-----------------------------------------------------------------------------
void LogSystem::create(const LogCreateInfo& createInfo)
{
#if defined(_WIN32)
	errno_t fileErr = fopen_s(&m_logFile, createInfo.fileName.c_str(), "w");
	if( fileErr != 0 || !m_logFile )
	{
		LogError("LogCreate() failed!!!");
		m_logFile = nullptr;
	}
#endif
}
//-----------------------------------------------------------------------------
void LogSystem::destroy()
{
#if defined(_WIN32)
	if( m_logFile )
	{
		fclose(m_logFile);
		m_logFile = nullptr;
	}
#endif
}
//-----------------------------------------------------------------------------