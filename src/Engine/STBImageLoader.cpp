#include "stdafx.h"
#include "STBImageLoader.h"
//-----------------------------------------------------------------------------
STBImageLoader::STBImageLoader(const char* fileName)
{
	//stbi_set_flip_vertically_on_load(verticallyFlip ? 1 : 0);
	const int desired—hannels = STBI_default;
	pixelData = stbi_load(fileName, &width, &height, &nrChannels, desired—hannels);
	if( !pixelData || nrChannels < STBI_grey || nrChannels > STBI_rgb_alpha || width == 0 || height == 0 )
	{
		isValid = false;
		return;
	}

	if( nrChannels == STBI_grey ) imageFormat = TexelsFormat::R_U8;
	else if( nrChannels == STBI_grey_alpha ) imageFormat = TexelsFormat::RG_U8;
	else if( nrChannels == STBI_rgb ) imageFormat = TexelsFormat::RGB_U8;
	else if( nrChannels == STBI_rgb_alpha ) imageFormat = TexelsFormat::RGBA_U8;

	isValid = true;
}
//-----------------------------------------------------------------------------
STBImageLoader::~STBImageLoader()
{
	stbi_image_free((void*)pixelData);
}
//-----------------------------------------------------------------------------