#pragma once

class EngineDevice;
class FileSystem;
class Window;
class Input;
class RenderSystem;
class WorldManager;
class PhysicsSystem;

class BaseClass
{
	friend EngineDevice;
public:
	void InitFromOther(BaseClass* other);

	void ExitRequest();
	FileSystem& GetFileSystem();
	const FileSystem& GetFileSystem() const;
	Window& GetWindow();
	const Window& GetWindow() const;
	Input& GetInput();
	const Input& GetInput() const;
	RenderSystem& GetRenderSystem();
	const RenderSystem& GetRenderSystem() const;
	WorldManager& GetWorldManager();
	const WorldManager& GetWorldManager() const;
	PhysicsSystem& GetPhysicsSystem();
	const PhysicsSystem& GetPhysicsSystem() const;


	void Print(const std::string& msg);
	void Warning(const std::string& msg);
	void Error(const std::string& msg);
	void Fatal(const std::string& msg);

	int GetWindowWidth() const;
	int GetWindowHeight() const;

protected:
	EngineDevice* m_engineDevice = nullptr;
};

using SystemRef = BaseClass;