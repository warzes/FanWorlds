#include "stdafx.h"
#include "Window.h"
//-----------------------------------------------------------------------------
#if defined(_WIN32)
#	pragma comment(lib, "shcore.lib")
#endif
//-----------------------------------------------------------------------------
static void GLFWErrorCallback(int error, const char *description) noexcept
{
	puts((std::string("GLFW error ") + std::to_string(error) + ": " + description).c_str());
}
//-----------------------------------------------------------------------------
bool Window::Create(const WindowCreateInfo& createInfo)
{
	Print("Window Create");

#if defined(_WIN32)
	if( createInfo.enablePerMonitorDPI )
	{
		// this needs to happen before glfwInit in order to override GLFW behavior
		SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	}
	else
	{
		SetProcessDpiAwareness(PROCESS_DPI_UNAWARE);
	}
#endif

	m_requestedVSync = createInfo.vsyncEnabled;

	if( glfwInit() != GLFW_TRUE )
	{
		Fatal("Failed to init GLFW.");
		return false;
	}
	Print("GLFW "
		+ std::to_string(GLFW_VERSION_MAJOR) + "."
		+ std::to_string(GLFW_VERSION_MINOR) + "."
		+ std::to_string(GLFW_VERSION_REVISION) + " initialized."
	);

	glfwSetErrorCallback(GLFWErrorCallback);
	glfwDefaultWindowHints();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_MAXIMIZED, maximized ? GLFW_TRUE : GLFW_FALSE);

	m_window = glfwCreateWindow(size.x, size.y, name.c_str(), nullptr, nullptr);
	if( !m_window )
	{
		Fatal("Failed to create window.");
		return false;
	}

	glfwSetFramebufferSizeCallback(m_window, FramebufferSizeCallback);

	glfwMakeContextCurrent(m_window);

	if( !gladLoadGL(glfwGetProcAddress) )
	{
		Fatal("Failed to load glad.");
		return false;
	}
	Print("GL_VERSION: " + std::string((const char*)glGetString(GL_VERSION)));
	Print("GL_SHADING_LANGUAGE_VERSION: " + std::string((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION)));
	Print("GL_VENDOR: " + std::string((const char*)glGetString(GL_VENDOR)));
	Print("GL_RENDERER: " + std::string((const char*)glGetString(GL_RENDERER)));

	glfwSetWindowUserPointer(m_window, this);
	return true;
}
//-----------------------------------------------------------------------------
void Window::Destroy()
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
	Print("Window Destroy");
}
//-----------------------------------------------------------------------------
void Window::Update()
{
}
//-----------------------------------------------------------------------------
void Window::Present()
{
}
//-----------------------------------------------------------------------------