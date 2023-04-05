#pragma once

#include "BaseClass.h"

class WorldManager : public BaseClass
{
	friend class EngineDevice;
public:
	void Init();
	void Close();

private:
	WorldManager() = default;
	WorldManager(WorldManager&&) = delete;
	WorldManager(const WorldManager&) = delete;
	WorldManager& operator=(WorldManager&&) = delete;
	WorldManager& operator=(const WorldManager&) = delete;
};