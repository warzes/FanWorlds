#pragma once

class EngineDevice;
class Window;
class Input;

class BaseClass
{
	friend EngineDevice;
public:
	void ExitRequest();

	Window& GetWindow();
	const Window& GetWindow() const;

	Input& GetInput();
	const Input& GetInput() const;

	void Print(const std::string& msg);
	void Warning(const std::string& msg);
	void Error(const std::string& msg);
	void Fatal(const std::string& msg);

	int GetWindowWidth() const;
	int GetWindowHeight() const;


protected:
	EngineDevice* m_engineDevice = nullptr;
};