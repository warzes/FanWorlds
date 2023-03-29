#include "stdafx.h"
#include "EngineDevice.h"
//-----------------------------------------------------------------------------
EngineDevice::EngineDevice(const EngineDeviceCreateInfo& createInfo)
{
}
//-----------------------------------------------------------------------------
EngineDevice::~EngineDevice()
{
	puts("System Destroy");
}
//-----------------------------------------------------------------------------
std::shared_ptr<EngineDevice> EngineDevice::Create(const EngineDeviceCreateInfo& createInfo)
{
	auto ptr = std::make_shared<EngineDevice>(createInfo);
	return ptr;
}
//-----------------------------------------------------------------------------
void EngineDevice::RunApp(std::shared_ptr<IApp> app)
{
	if( !app || m_isExitRequested ) return;

	// Init
	m_currentApp = app;
	m_currentApp->m_engineDevice = shared_from_this();

	if( m_currentApp->Create() )
	{
		/*while( !m_isExitRequested )
		{

		}*/
	}
	// Destroy App
	m_currentApp->Destroy();
	m_currentApp = nullptr;
}
//-----------------------------------------------------------------------------