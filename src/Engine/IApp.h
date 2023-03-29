#pragma once

#include "BaseClass.h"

class IApp : public BaseClass
{
public:
	virtual ~IApp() = default;

	virtual bool Create() = 0;
	virtual void Destroy() = 0;

private:
};