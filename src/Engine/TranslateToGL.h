#pragma once

#include "RenderResource.h"

inline GLenum TranslateToGL(ResourceUsage usage)
{
	switch( usage )
	{
	case ResourceUsage::Static:  return GL_STATIC_DRAW;
	case ResourceUsage::Dynamic: return GL_DYNAMIC_DRAW;
	case ResourceUsage::Stream:  return GL_STREAM_DRAW;
	}
	return 0;
}
//-----------------------------------------------------------------------------
inline GLenum TranslateToGL(PrimitiveTopology p)
{
	switch( p )
	{
	case PrimitiveTopology::Lines:     return GL_LINES;
	case PrimitiveTopology::Triangles: return GL_TRIANGLES;
	case PrimitiveTopology::Points:    return GL_POINTS;
	}
	return 0;
}
//-----------------------------------------------------------------------------
inline GLint TranslateToGL(TextureAddressMode wrap)
{
	switch( wrap )
	{
	case TextureAddressMode::Repeat:         return GL_REPEAT;
	case TextureAddressMode::MirroredRepeat: return GL_MIRRORED_REPEAT;
	case TextureAddressMode::ClampToEdge:    return GL_CLAMP_TO_EDGE;
#if !defined(__EMSCRIPTEN__)
	case TextureAddressMode::ClampToBorder:  return GL_CLAMP_TO_BORDER;
#endif
	}
	return 0;
}
//-----------------------------------------------------------------------------
inline GLint TranslateToGL(TextureMinFilter filter)
{
	switch( filter )
	{
	case TextureMinFilter::Nearest:              return GL_NEAREST;
	case TextureMinFilter::Linear:               return GL_LINEAR;
	case TextureMinFilter::NearestMipmapNearest: return GL_NEAREST_MIPMAP_NEAREST;
	case TextureMinFilter::NearestMipmapLinear:  return GL_NEAREST_MIPMAP_LINEAR;
	case TextureMinFilter::LinearMipmapNearest:  return GL_LINEAR_MIPMAP_NEAREST;
	case TextureMinFilter::LinearMipmapLinear:   return GL_LINEAR_MIPMAP_LINEAR;
	}
	return 0;
}
//-----------------------------------------------------------------------------
inline constexpr GLint TranslateToGL(TextureMagFilter filter)
{
	switch( filter )
	{
	case TextureMagFilter::Nearest: return GL_NEAREST;
	case TextureMagFilter::Linear:  return GL_LINEAR;
	}
	return 0;
}
inline constexpr unsigned SizeIndexBuffer(IndexBufferFormat format)
{
	switch (format)
	{
	case IndexBufferFormat::Uint16: return sizeof(uint16_t);
	case IndexBufferFormat::Uint32: return sizeof(uint32_t);
	}
}
//-----------------------------------------------------------------------------
inline bool GetTextureFormatType(TexelsFormat inFormat, GLenum textureType, GLenum& format, GLint& internalFormat, GLenum& oglType)
{
	if( inFormat == TexelsFormat::R_U8 )
	{
		format = GL_RED;
		internalFormat = GL_R8;
		oglType = GL_UNSIGNED_BYTE;
	}
	else if( inFormat == TexelsFormat::RG_U8 )
	{
#if defined(_WIN32)
		format = GL_RG;
		internalFormat = GL_RG8;
		oglType = GL_UNSIGNED_BYTE;
		const GLint swizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_GREEN };
		glTexParameteriv(textureType, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask); // TODO: ����� ���� �������� � ����������, ����� ������ ������� stb � ��������� ������� �������
#endif // _WIN32
#if defined(__EMSCRIPTEN__)
		Fatal("TexelsFormat::RG_U8 not support in web platform");
		return false;
#endif
	}
	else if( inFormat == TexelsFormat::RGB_U8 )
	{
		format = GL_RGB;
		internalFormat = GL_RGB8;
		oglType = GL_UNSIGNED_BYTE;
	}
	else if( inFormat == TexelsFormat::RGBA_U8 )
	{
		format = GL_RGBA;
		internalFormat = GL_RGBA8;
		oglType = GL_UNSIGNED_BYTE;
	}
	else if( inFormat == TexelsFormat::Depth_U16 )
	{
		format = GL_DEPTH_COMPONENT;
		internalFormat = GL_DEPTH_COMPONENT16;
		oglType = GL_UNSIGNED_SHORT;
	}
	else if( inFormat == TexelsFormat::DepthStencil_U16 )
	{
		format = GL_DEPTH_STENCIL;
		internalFormat = GL_DEPTH24_STENCIL8;
		oglType = GL_UNSIGNED_SHORT;
	}
	else if( inFormat == TexelsFormat::Depth_U24 )
	{
		format = GL_DEPTH_COMPONENT;
		internalFormat = GL_DEPTH_COMPONENT24;
		oglType = GL_UNSIGNED_INT;
	}
	else if( inFormat == TexelsFormat::DepthStencil_U24 )
	{
		format = GL_DEPTH_STENCIL;
		internalFormat = GL_DEPTH24_STENCIL8;
		oglType = GL_UNSIGNED_INT;
	}
	else
	{
		//Error("unknown texture format");
		return false;
	}
	return true;
}