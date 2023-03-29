#pragma once

#include "LogSystem.h"
#include "EngineTimestamp.h"
#include "Window.h"
#include "IApp.h"


struct EngineDeviceCreateInfo
{
	LogCreateInfo log;
	WindowCreateInfo window;
};

class EngineDevice
{
	friend BaseClass;
public:
	EngineDevice() = default;
	~EngineDevice();

	static std::shared_ptr<EngineDevice> Create(const EngineDeviceCreateInfo& createInfo);

	void RunApp(std::shared_ptr<IApp> startApp);

private:
	EngineDevice(EngineDevice&&) = delete;
	EngineDevice(const EngineDevice&) = delete;
	EngineDevice& operator=(EngineDevice&&) = delete;
	EngineDevice& operator=(const EngineDevice&) = delete;

	void init(const EngineDeviceCreateInfo& createInfo);
	void close();

	void update();
	void render();
	void present();

	std::shared_ptr<IApp> m_currentApp = nullptr;
	LogSystem m_logSystem;
	Window m_window;

	EngineTimestamp m_timestamp;

	bool m_isExitRequested = true;
};