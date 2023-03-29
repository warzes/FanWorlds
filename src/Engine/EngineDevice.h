#pragma once

#include "LogSystem.h"
#include "EngineTimestamp.h"
#include "IApp.h"


struct EngineDeviceCreateInfo
{
	LogCreateInfo log;
};

class EngineDevice : public std::enable_shared_from_this<EngineDevice>
{
	friend BaseClass;
public:
	EngineDevice(const EngineDeviceCreateInfo& createInfo);
	~EngineDevice();

	static std::shared_ptr<EngineDevice> Create(const EngineDeviceCreateInfo& createInfo);

	void RunApp(std::shared_ptr<IApp> startApp);

private:
	EngineDevice(EngineDevice&&) = delete;
	EngineDevice(const EngineDevice&) = delete;
	EngineDevice& operator=(EngineDevice&&) = delete;
	EngineDevice& operator=(const EngineDevice&) = delete;

	void update();
	void render();
	void present();

	std::shared_ptr<IApp> m_currentApp = nullptr;
	LogSystem m_logSystem;
	bool m_isExitRequested = false;

	EngineTimestamp m_timestamp;
};