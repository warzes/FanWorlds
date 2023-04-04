#pragma once

#include "LogSystem.h"
#include "EngineTimestamp.h"
#include "FileSystem.h"
#include "Window.h"
#include "Input.h"
#include "RenderSystem.h"
#include "GraphicsSystem.h"
#include "WorldManager.h"
#include "PhysicsSystem.h"
#include "IApp.h"

struct EngineDeviceCreateInfo
{
	LogCreateInfo log;
	WindowCreateInfo window;
	RenderCreateInfo render;
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
	FileSystem m_fileSystem;
	Window m_window;
	Input m_input;
	RenderSystem m_renderSystem;
	GraphicsSystem m_graphicsSystem;
	WorldManager m_worldManager;
	PhysicsSystem m_physicsSystem;

	EngineTimestamp m_timestamp;

	bool m_isExitRequested = true;
};