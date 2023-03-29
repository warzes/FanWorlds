#include "stdafx.h"
#include "EngineDevice.h"
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
	m_logSystem.m_engineDevice = this;
	m_logSystem.create(createInfo.log);
	m_logSystem.LogPrint("EngineDevice Create");

	m_window.m_engineDevice = this;
	if( !m_window.Create(createInfo.window) )
		return;

	m_input.m_engineDevice = this;
	m_input.Init();

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
	m_input.Update();

	m_currentApp->Update(m_timestamp.ElapsedTime);

	m_timestamp.UpdateAverageFrameTime();
}
//-----------------------------------------------------------------------------
void EngineDevice::render()
{
	glViewport(0, 0, 1024, 768);
	glClearColor(0.2f, 0.4f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_currentApp->Render();
}
//-----------------------------------------------------------------------------
void EngineDevice::present()
{
	m_window.Present();
}
//-----------------------------------------------------------------------------