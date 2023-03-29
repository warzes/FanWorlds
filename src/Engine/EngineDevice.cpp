#include "stdafx.h"
#include "EngineDevice.h"
unrimp
haru
https ://open.gl
https://github.com/OpenGL-Graphics/opengl-utils
https://github.com/OpenGL-Graphics/first-person-shooter
PawsForAdventure
https ://discord.com/channels/794280341149712404/1042555400844750858

//-----------------------------------------------------------------------------
EngineDevice::~EngineDevice()
{
	close();
}
//-----------------------------------------------------------------------------
std::shared_ptr<EngineDevice> EngineDevice::Create(const EngineDeviceCreateInfo& createInfo)
{
	auto ptr = std::make_shared<EngineDevice>();
	ptr->init(createInfo);
	return ptr;
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
	m_currentApp->m_engineDevice = this;

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
void EngineDevice::init(const EngineDeviceCreateInfo& createInfo)
{
	m_logSystem.create(createInfo.log);
	m_logSystem.LogPrint("EngineDevice Create");

	m_window.m_engineDevice = this;
	if( !m_window.Create(createInfo.window) )
		return;

	m_isExitRequested = false;
}
//-----------------------------------------------------------------------------
void EngineDevice::close()
{
	m_window.Destroy();
	m_window.m_engineDevice = nullptr;
	m_logSystem.destroy();
	m_logSystem.LogPrint("EngineDevice Destroy");
}
//-----------------------------------------------------------------------------
void EngineDevice::update()
{
	m_window.Update();
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
	m_window.Present();
}
//-----------------------------------------------------------------------------