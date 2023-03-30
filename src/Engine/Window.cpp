#include "stdafx.h"
#include "Window.h"
//-----------------------------------------------------------------------------
static void GLFWErrorCallback(int error, const char *description) noexcept
{
	puts((std::string("GLFW error ") + std::to_string(error) + ": " + description).c_str());
}
//-----------------------------------------------------------------------------
void GLFWFramebufferSizeCallback(GLFWwindow* window, int width, int height) noexcept
{
	const auto app = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if( app == nullptr ) return;
	app->m_windowWidth = width;
	app->m_windowHeight = width;
}
//-----------------------------------------------------------------------------
bool Window::Create(const WindowCreateInfo& createInfo)
{
	Print("Window Create");

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

	// OpenGL Config
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_FALSE);
#if defined(_DEBUG)
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#else
	glfwWindowHint(GLFW_CONTEXT_NO_ERROR, GLFW_TRUE);
#endif

	// Window Config
	glfwWindowHint(GLFW_RESIZABLE, createInfo.resizable ? GL_TRUE : GL_FALSE);
	glfwWindowHint(GLFW_MAXIMIZED, createInfo.maximized ? GLFW_TRUE : GLFW_FALSE);

	m_window = glfwCreateWindow(createInfo.width, createInfo.height, createInfo.title.c_str(), nullptr, nullptr);
	if( !m_window )
	{
		Fatal("GLFW: Failed to initialize Window");
		return false;
	}
	glfwSetWindowUserPointer(m_window, this);

	glfwSetFramebufferSizeCallback(m_window, GLFWFramebufferSizeCallback);

	glfwMakeContextCurrent(m_window);

	if( !gladLoadGL((GLADloadfunc)glfwGetProcAddress) )
	{
		Fatal("GLAD: Cannot load OpenGL extensions.");
		return false;
	}

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