#pragma once

#include "RenderResource.h"

class STBImageLoader
{
public:
	STBImageLoader(const char* fileName);
	~STBImageLoader();

	int width = 0;
	int height = 0;
	int nrChannels = 0;
	stbi_uc* pixelData = nullptr;
	TexelsFormat imageFormat = TexelsFormat::RGBA_U8;

	bool isValid = false;
};