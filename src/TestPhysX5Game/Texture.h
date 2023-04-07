#pragma once

#include "MoveOnly.h"

class RenderSystem;


class Texture2
{
public:
	MOVE_ONLY(Texture2);

	enum class Wrap
	{
		Repeat,
		Clamp,
		MirrorRepeat
	};

	Texture2() = delete;
	Texture2(RenderSystem& renderSystem) : m_renderSystem(renderSystem) {}
	Texture2(
		RenderSystem& renderSystem,
		const glm::ivec2 &size,
		const unsigned char *data,
		Wrap wrap = Wrap::Repeat,
		bool filter = false,
		bool mipmaps = false
	);
	~Texture2();

	static Texture2 FromFile(
		RenderSystem& renderSystem,
		const std::string &filename,
		Wrap wrap = Wrap::Repeat,
		bool filter = false,
		bool mipmaps = false
	);

	static void ClearCache();

	static Texture2 *LoadToCache(SystemRef& systemRef, const std::string &filename);

	[[nodiscard]] const glm::ivec2 &Size() const { return m_size; }

	void Bind(GLuint unit) const;

private:
	RenderSystem& m_renderSystem;
	MoveOnly<glm::ivec2> m_size;
	MoveOnly<GLuint> m_texture;
};