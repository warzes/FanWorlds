#pragma once

class FileSystem;

class ImageFile
{
public:
	ImageFile() = delete;
	ImageFile(ImageFile &&other) noexcept { Swap(other); }
	ImageFile(FileSystem& fileSystem, const std::string &filename);
	~ImageFile() { Release(); }

	ImageFile(const ImageFile&) = delete;
	ImageFile& operator=(const ImageFile&) = delete;

	ImageFile& operator=(ImageFile&& other) noexcept
	{
		if( this != &other )
		{
			Release();
			Swap(other);
		}
		return *this;
	}

	void Release();
	void Swap(ImageFile& other) noexcept;

	[[nodiscard]] uint32_t GetWidth() const { return m_width; }
	[[nodiscard]] uint32_t GetHeight() const { return m_height; }
	[[nodiscard]] const unsigned char *GetData() const { return m_data; }

private:
	uint32_t       m_width = 0;
	uint32_t       m_height = 0;
	unsigned char* m_data = nullptr;
};