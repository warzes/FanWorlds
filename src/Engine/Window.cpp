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
//void GLFWWindowSizeCallback(GLFWwindow* /*window*/, int width, int height) noexcept
//{
//	//WindowWidth = width;
//	//WindowHeight = height;
//}
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
		Fatal("GLFW: Failed to initialize GLFW.");
		return false;
	}
	Print("GLFW "
		+ std::to_string(GLFW_VERSION_MAJOR) + "."
		+ std::to_string(GLFW_VERSION_MINOR) + "."
		+ std::to_string(GLFW_VERSION_REVISION) + " initialized."
	);
	glfwSetErrorCallback(GLFWErrorCallback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_FALSE);
#if defined(_DEBUG)
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#else
	glfwWindowHint(GLFW_CONTEXT_NO_ERROR, GLFW_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, createInfo.resizable ? GL_TRUE : GL_FALSE);

	m_window = glfwCreateWindow(createInfo.width, createInfo.height, createInfo.title.c_str(), nullptr, nullptr);
	if( !m_window )
	{
		Fatal("GLFW: Failed to initialize Window");
		return false;
	}
	glfwSetWindowUserPointer(m_window, this);

	//glfwSetWindowSizeCallback(m_window, GLFWWindowSizeCallback);

	glfwMakeContextCurrent(m_window);

	if( !gladLoadGL(glfwGetProcAddress) )
	{
		Fatal("GLAD: Cannot load OpenGL extensions.");
		return false;
	}
	Print("OpenGL: OpenGL device information:");
	Print("    > Vendor:   " + std::string((const char*)glGetString(GL_VENDOR)));
	Print("    > Renderer: " + std::string((const char*)glGetString(GL_RENDERER)));
	Print("    > Version:  " + std::string((const char*)glGetString(GL_VERSION)));
	Print("    > GLSL:     " + std::string((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION)));

	glfwSwapInterval(m_requestedVSync ? 1 : 0);

	glfwGetFramebufferSize(m_window, &m_windowWidth, &m_windowHeight);

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
	if (glfwWindowShouldClose(m_window) == GLFW_TRUE) BaseClass::ExitRequest();
}
//-----------------------------------------------------------------------------
void Window::Present()
{
	glfwSwapBuffers(m_window);
}
//-----------------------------------------------------------------------------
int Window::GetWidth() const
{
	return m_windowWidth;
}
//-----------------------------------------------------------------------------
int Window::GetHeight() const
{
	return m_windowHeight;
}
//-----------------------------------------------------------------------------