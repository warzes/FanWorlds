#pragma once

#include "MoveOnly.h"

class RenderSystem;
class BaseClass;

class Texture
{
public:
	MOVE_ONLY(Texture);

	enum class Wrap
	{
		Repeat,
		Clamp,
		MirrorRepeat
	};

	Texture() = delete;
	Texture(RenderSystem& renderSystem) : m_renderSystem(renderSystem) {}
	Texture(
		RenderSystem& renderSystem,
		const glm::ivec2 &size,
		const unsigned char *data,
		Wrap wrap = Wrap::Repeat,
		bool filter = false,
		bool mipmaps = false
	);
	~Texture();

	static Texture FromFile(
		RenderSystem& renderSystem,
		const std::string &filename,
		Wrap wrap = Wrap::Repeat,
		bool filter = false,
		bool mipmaps = false
	);

	static void ClearCache();

	static Texture *LoadToCache(BaseClass& baseClass, const std::string &filename);

	[[nodiscard]] const glm::ivec2 &Size() const { return m_size; }

	void Bind(GLuint unit) const;

private:
	RenderSystem& m_renderSystem;
	MoveOnly<glm::ivec2> m_size;
	MoveOnly<GLuint> m_texture;
};