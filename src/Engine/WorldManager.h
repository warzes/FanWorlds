#pragma once

#include "BaseClass.h"

class WorldManager : public BaseClass
{
	friend class EngineDevice;
public:
	void Init();
	void Close();
};