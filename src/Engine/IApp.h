#pragma once

#include "BaseClass.h"

class IApp : public BaseClass
{
public:
	virtual ~IApp() = default;

	virtual bool Create() = 0;
	virtual void Destroy() = 0;

	virtual void Render() = 0;
	virtual void Update(float elapsedTimeSeconds) = 0;

	// return value is true if the event was consumed by this render pass, false if it should be passed on
	virtual bool KeyboardUpdate(int key, int scancode, int action, int mods) { return false; }
	virtual bool KeyboardCharInput(unsigned int unicode, int mods) { return false; }
	virtual bool MousePosUpdate(double xpos, double ypos) { return false; }
	virtual bool MouseScrollUpdate(double xoffset, double yoffset) { return false; }
	virtual bool MouseButtonUpdate(int button, int action, int mods) { return false; }
};