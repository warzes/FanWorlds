#pragma once

class EngineDevice;

class BaseClass
{
	friend EngineDevice;
public:
	void ExitRequest();

	void Print(const std::string& msg);
	void Warning(const std::string& msg);
	void Error(const std::string& msg);
	void Fatal(const std::string& msg);

protected:
	std::shared_ptr<EngineDevice> m_engineDevice = nullptr;
};