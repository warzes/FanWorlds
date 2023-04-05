#pragma once

#include "BaseClass.h"

namespace FileSystemUtils
{
	std::optional<std::vector<uint8_t>> FileToMemory(const char* fileName, unsigned int* bytesRead = nullptr);

	bool FileExists(const char* fileName);     // Check if file exists

	// Get pointer to extension for a filename string (includes the dot: .png)
	const char* GetFileExtension(const char* fileName);
	// Get pointer to filename for a path string
	const char* GetFileName(const char* filePath);
}

class FileSystem : public BaseClass
{
	friend class EngineDevice;
public:
	bool Init();
	void Close();

	FileSystem &Mount(const std::string &newDir, const std::string &mountPoint, bool appendToPath = true);

	std::string Read(const std::string &filename);

private:
	FileSystem() = default;
	FileSystem(FileSystem&&) = delete;
	FileSystem(const FileSystem&) = delete;
	FileSystem& operator=(FileSystem&&) = delete;
	FileSystem& operator=(const FileSystem&) = delete;
};