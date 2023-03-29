#include "stdafx.h"
#include "EngineDevice.h"
//-----------------------------------------------------------------------------
EngineDevice::EngineDevice(const EngineDeviceCreateInfo& createInfo)
{
	m_logSystem.create(createInfo.log);
	m_logSystem.LogPrint("EngineDevice Create");
}
//-----------------------------------------------------------------------------
EngineDevice::~EngineDevice()
{
	m_logSystem.destroy();

	m_logSystem.LogPrint("EngineDevice Destroy");
}
//-----------------------------------------------------------------------------
std::shared_ptr<EngineDevice> EngineDevice::Create(const EngineDeviceCreateInfo& createInfo)
{
	return std::make_shared<EngineDevice>(createInfo);
}
//-----------------------------------------------------------------------------
void EngineDevice::RunApp(std::shared_ptr<IApp> app)
{
	assert(app);
	assert(!m_isExitRequested);
	if( !app || m_isExitRequested ) return;

	m_logSystem.LogPrint("EngineDevice RunApp");

	// Init
	m_currentApp = app;
	m_currentApp->m_engineDevice = shared_from_this();

	if( m_currentApp->Create() )
	{
		m_timestamp.PreviousFrameTimestamp = EngineTimestamp::GetTime();

		while( !m_isExitRequested )
		{
			update();
			render();
			present();
		}
	}
	// Destroy App
	m_currentApp->Destroy();
	m_currentApp = nullptr;
}
//-----------------------------------------------------------------------------
void EngineDevice::update()
{
	m_timestamp.Update();
	m_currentApp->Update(m_timestamp.ElapsedTime);


	m_timestamp.UpdateAverageFrameTime();
}
//-----------------------------------------------------------------------------
void EngineDevice::render()
{
	m_currentApp->Render();
}
//-----------------------------------------------------------------------------
void EngineDevice::present()
{
}
//-----------------------------------------------------------------------------