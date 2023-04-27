#include "stdafx.h"
#include "EngineDevice.h"
//-----------------------------------------------------------------------------
#if defined(_MSC_VER)
#	pragma comment( lib, "3rdparty.lib" )
#	pragma comment( lib, "PhysX5Lib.lib" )
#	pragma comment( lib, "BulletLib.lib" )
#	pragma comment( lib, "BounceLib.lib" )
#	pragma comment( lib, "solid3Lib.lib" )
#endif
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
			extern bool IsError; // ==> Error.cpp
			extern bool IsFatal; // ==> Error.cpp
			extern std::string ErrorText; // ==> Error.cpp
			
			if( IsFatal )
			{
				m_logSystem.Fatal(ErrorText);
				ErrorText = "";
				IsFatal = false;
				break;
			}
			if( IsError )
			{
				m_logSystem.Error(ErrorText);
				ErrorText = "";
				IsError = false;
			}

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

	m_fileSystem.m_engineDevice = this;
	if( !m_fileSystem.Init() )
		return;

	m_window.m_engineDevice = this;
	if( !m_window.Create(createInfo.window) )
		return;

	m_input.m_engineDevice = this;
	m_input.Init();

	m_renderSystem.m_engineDevice = this;
	m_renderSystem.Init(createInfo.render);

	m_graphicsSystem.m_engineDevice = this;
	m_graphicsSystem.Init();

	m_physicsSystem.m_engineDevice = this;
	if( !m_physicsSystem.Create() )
		return;

	m_worldManager.m_engineDevice = this;
	m_worldManager.Init();

	m_isExitRequested = false;
}
//-----------------------------------------------------------------------------
void EngineDevice::close()
{
	m_logSystem.LogPrint("EngineDevice Destroy");
	
	m_worldManager.Close();
	m_physicsSystem.Destroy();
	m_graphicsSystem.Close();
	m_renderSystem.Close();
	m_window.Destroy();
	m_fileSystem.Close();
	m_logSystem.destroy();
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
	m_renderSystem.BeginFrame();
	m_currentApp->Render();
	m_renderSystem.EndFrame();
}
//-----------------------------------------------------------------------------
void EngineDevice::present()
{
	m_window.Present();
}
//-----------------------------------------------------------------------------