#pragma once

#include "MoveOnly.h"

class FileSystem;

class ImageFile
{
public:
	explicit ImageFile(FileSystem& fileSystem, const std::string &filename);
	~ImageFile();

	[[nodiscard]] const glm::ivec2 &Size() const { return m_size; }
	[[nodiscard]] const unsigned char *Data() const { return m_data; }

private:
	MoveOnly<glm::ivec2> m_size;
	MoveOnly<unsigned char*> m_data;
};