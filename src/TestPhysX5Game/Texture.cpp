#include "stdafx.h"
#include "Texture.h"
#include "ImageFile.h"
//-----------------------------------------------------------------------------
static std::unordered_map<std::string, Texture2> s_textureCache;
//-----------------------------------------------------------------------------
Texture2::Texture2(RenderSystem& renderSystem, const glm::ivec2 &size, const unsigned char *data, Wrap wrap, bool filter, bool mipmaps)
	: m_renderSystem(renderSystem)
{
	m_size = size;

	glCreateTextures(GL_TEXTURE_2D, 1, &m_texture);
	glTextureStorage2D(m_texture, 1, GL_RGBA8, size.x, size.y);
	glTextureSubImage2D(m_texture, 0, 0, 0, size.x, size.y, GL_RGBA, GL_UNSIGNED_BYTE, data);

	if( filter )
	{
		glTextureParameteri(m_texture, GL_TEXTURE_MIN_FILTER, mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
		glTextureParameteri(m_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		glTextureParameteri(m_texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(m_texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	switch( wrap )
	{
	case Wrap::Repeat:
		glTextureParameteri(m_texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
		break;
	case Wrap::Clamp:
		glTextureParameteri(m_texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		break;
	case Wrap::MirrorRepeat:
		glTextureParameteri(m_texture, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTextureParameteri(m_texture, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		break;
	}

	if( mipmaps )
	{
		glGenerateTextureMipmap(m_texture);
	}
}
//-----------------------------------------------------------------------------
Texture2 Texture2::FromFile(RenderSystem& renderSystem, const std::string &filename, Texture2::Wrap wrap, bool filter, bool mipmaps)
{
	ImageFile image(renderSystem.GetFileSystem(), filename);
	if( !image.GetData() )
	{
		renderSystem.Error("Failed to load texture from file: " + filename);
		return { renderSystem };
	}
	return { renderSystem, {image.GetWidth(), image.GetHeight()}, image.GetData(), wrap, filter, mipmaps};
}
//-----------------------------------------------------------------------------
void Texture2::ClearCache()
{
	s_textureCache.clear();
}
//-----------------------------------------------------------------------------
Texture2 *Texture2::LoadToCache(SystemRef& systemRef, const std::string &filename)
{
	auto pair = s_textureCache.find(filename);
	if( pair == s_textureCache.end() )
	{
		systemRef.Print("Caching texture: " + filename);
		pair = s_textureCache.emplace(filename, FromFile(systemRef.GetRenderSystem(), filename)).first;
	}
	return &pair->second;
}
//-----------------------------------------------------------------------------
Texture2::~Texture2()
{
	if( m_texture ) glDeleteTextures(1, &m_texture);
}
//-----------------------------------------------------------------------------
void Texture2::Bind(GLuint unit) const
{
	glBindTextureUnit(unit, m_texture);
}
//-----------------------------------------------------------------------------