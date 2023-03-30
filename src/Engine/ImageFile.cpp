#include "stdafx.h"
#include "ImageFile.h"
#include "FileSystem.h"
//-----------------------------------------------------------------------------
ImageFile::ImageFile(FileSystem& fileSystem, const std::string &filename)
{
	stbi_set_flip_vertically_on_load(true);
	std::string bytes = fileSystem.ReadFile(filename);
	m_data = stbi_load_from_memory(
		reinterpret_cast<const stbi_uc*>(bytes.data()),
		static_cast<int>(bytes.length()),
		&m_size->x,
		&m_size->y,
		nullptr,
		4
	);
}
//-----------------------------------------------------------------------------
ImageFile::~ImageFile()
{
	stbi_image_free(m_data);
}
//-----------------------------------------------------------------------------