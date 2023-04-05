#pragma once

#include "BaseClass.h"

struct WindowCreateInfo
{
	int width = 1024;
	int height = 768;
	std::string title = "Game";
	bool resizable = true;
	bool maximized = true;

	// set to true to enable DPI scale factors to be computed per monitor this will keep the on-screen window size in pixels constant
	// if set to false, the DPI scale factors will be constant but the system may scale the contents of the window based on DPI
	// note that the backbuffer size is never updated automatically; if the app wishes to scale up rendering based on DPI, then it must set this to true nd respond to DPI scale factor changes by resizing the backbuffer explicitly
	bool enablePerMonitorDPI = false;

	bool vsyncEnabled = false;
};

class Window : public BaseClass
{
	friend class EngineDevice;
	friend class Input;
	friend void GLFWFramebufferSizeCallback(GLFWwindow*, int, int) noexcept;
public:
	bool Create(const WindowCreateInfo& createInfo);
	void Destroy();

	void Update();
	void Present();

	int GetWidth() const;
	int GetHeight() const;

private:
	Window() = default;
	Window(Window&&) = delete;
	Window(const Window&) = delete;
	Window& operator=(Window&&) = delete;
	Window& operator=(const Window&) = delete;

	GLFWwindow* m_window = nullptr;
	bool m_requestedVSync = false;
	int m_windowWidth = 0;
	int m_windowHeight = 0;
};