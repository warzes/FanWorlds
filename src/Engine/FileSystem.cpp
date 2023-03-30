#include "stdafx.h"
#include "FileSystem.h"
//-----------------------------------------------------------------------------
std::optional<std::vector<uint8_t>> FileSystemUtils::FileToMemory(const char* fileName, unsigned int* bytesRead)
{
	if (!fileName || !fileName[0])
	{
		//LogError("File name provided is not valid");
		return std::nullopt;
	}

	FILE* file = nullptr;
	errno_t err;
	err = fopen_s(&file, fileName, "rb");
	if (err != 0 || !file)
	{
		//LogError("Failed to open file '" + std::string(fileName) + "'");
		return std::nullopt;
	}

	if (fseek(file, 0L, SEEK_END) != 0)
	{
		//LogError("Unable to seek file '" + std::string(fileName) + "'");
		fclose(file);
		return std::nullopt;
	}

	const long fileLen = ftell(file);
	if (fileLen <= 0)
	{
		//LogError("Failed to read file '" + std::string(fileName) + "'");
		fclose(file);
		return std::nullopt;
	}

	if (fseek(file, 0, SEEK_SET) != 0)
	{
		//LogError("Unable to seek file '" + std::string(fileName) + "'");
		fclose(file);
		return std::nullopt;
	}

	std::vector<uint8_t> contents(static_cast<size_t>(fileLen + 1));
	if (contents.empty() || contents.size() <= 1)
	{
		//LogError("Memory error!");
		fclose(file);
		return std::nullopt;
	}

	const size_t count = fread(contents.data(), sizeof(unsigned char), static_cast<size_t>(fileLen), file);
	if (count == 0 || ferror(file))
	{
		//LogError("Read error");
		fclose(file);
		return std::nullopt;
	}
	contents[static_cast<size_t>(fileLen)] = 0;

	*bytesRead = static_cast<unsigned int>(count);

	fclose(file);
	return contents;
}
//-----------------------------------------------------------------------------
bool FileSystemUtils::FileExists(const char* fileName)
{
	bool result = false;

#if defined(_WIN32)
	if (_access(fileName, 0) != -1) result = true;
#else
	if (access(fileName, F_OK) != -1) result = true;
#endif

	// NOTE: Alternatively, stat() can be used instead of access()
	//#include <sys/stat.h>
	//struct stat statbuf;
	//if (stat(filename, &statbuf) == 0) result = true;

	return result;
}
//-----------------------------------------------------------------------------
const char* FileSystemUtils::GetFileExtension(const char* fileName)
{
	// TODO: бывают случаи когда точка используется в имени файла например file.foo.ext
	// сейчас это не будет работать
	const char* dot = strrchr(fileName, '.');
	if (!dot || dot == fileName) return nullptr;
	return dot;
}
//-----------------------------------------------------------------------------
const char* FileSystemUtils::GetFileName(const char* filePath)
{
	const char* latestMatch = nullptr;
	for (; filePath = strpbrk(filePath, "\\/"), filePath != nullptr; latestMatch = filePath++) {}
	const char* fileName = latestMatch;
	if (!fileName) return filePath;
	return fileName + 1;
}
//-----------------------------------------------------------------------------
std::string GetFileSystemLastError()
{
	return PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
}
//-----------------------------------------------------------------------------
bool FileSystem::Init()
{
	if( !PHYSFS_init(__argv[0]))
	{
		Error("Failed to init Physfs: " + GetFileSystemLastError());
		return false;
	}
	Print("Physfs " 
		+ std::to_string(PHYSFS_VER_MAJOR) + "." 
		+ std::to_string(PHYSFS_VER_MINOR) + "."
		+ std::to_string(PHYSFS_VER_PATCH));

	return true;
}
//-----------------------------------------------------------------------------
void FileSystem::Close()
{
	if( !PHYSFS_deinit() )
	{
		Error("Failed to destroy Physfs: " + GetFileSystemLastError());
	}
}
//-----------------------------------------------------------------------------
FileSystem &FileSystem::Mount(const std::string &newDir, const std::string &mountPoint, bool appendToPath)
{
	if( !PHYSFS_mount(newDir.c_str(), mountPoint.c_str(), appendToPath) )
	{
		Error("Physfs failed to mount " + newDir + " at " + mountPoint + ": " + GetFileSystemLastError());
	}
	else
	{
		Print("Physfs mount %s at %s" + newDir +" at " + mountPoint);
	}
	return *this;
}
//-----------------------------------------------------------------------------
std::string FileSystem::ReadFile(const std::string &filename)
{
	PHYSFS_File *file = PHYSFS_openRead(filename.c_str());
	if( !file )
	{
		Error("Physfs failed to open file " + filename + ": " + GetFileSystemLastError());
		return {};
	}
	PHYSFS_sint64 length = PHYSFS_fileLength(file);
	if( length == -1 )
	{
		Error("Physfs failed to get file size " + filename + ": " + GetFileSystemLastError());
		return {};
	}
	std::string bytes(length, '\0');
	if( PHYSFS_readBytes(file, bytes.data(), length) == -1 )
	{
		Error("Physfs failed to read file " + filename + ": " + GetFileSystemLastError());
	}
	return bytes;
}
//-----------------------------------------------------------------------------