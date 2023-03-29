#pragma once

#include "BaseClass.h"

class LogSystem : public BaseClass
{
	friend class EngineDevice;
public:
	~LogSystem();

	void LogPrint(const std::string& msg);
	void LogWarning(const std::string& msg);
	void LogError(const std::string& msg);
	void LogFatal(const std::string& msg);

private:
	LogSystem() = default;
	LogSystem(LogSystem&&) = delete;
	LogSystem(const LogSystem&) = delete;
	LogSystem& operator=(LogSystem&&) = delete;
	LogSystem& operator=(const LogSystem&) = delete;

	struct LogCreateInfo
	{
		std::string fileName;
	};

	void create(const LogCreateInfo& createInfo);
	void destroy();

	FILE* m_logFile = nullptr;
};