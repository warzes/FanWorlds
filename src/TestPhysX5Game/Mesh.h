#pragma once

#include "MoveOnly.h"

template<typename V>
class Mesh2
{
public:
	MOVE_ONLY(Mesh2)

		Mesh2() = default;

	// static
	Mesh2(const size_t count, const V *data, const GLenum mode)
	{
		m_count = static_cast<GLsizei>(count);
		m_mode = mode;

		glCreateBuffers(1, &m_vbo);
		glNamedBufferStorage(m_vbo, count * sizeof(V), data, 0);

		glCreateVertexArrays(1, &m_vao);
		glVertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, sizeof(V));

		V::SetupVertexArray(m_vao);
	}

	// dynamic recreate
	explicit Mesh2(const GLenum mode)
	{
		m_mode = mode;

		glCreateBuffers(1, &m_vbo);

		glCreateVertexArrays(1, &m_vao);
		glVertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, sizeof(V));

		V::SetupVertexArray(m_vao);
	}

	template<size_t Size>
	Mesh2(const V(&data)[Size], const GLenum mode)
		: Mesh2(Size, data, mode)
	{
	}

	Mesh2(const std::vector<V> &vertices, const GLenum mode)
		: Mesh2(vertices.size(), vertices.data(), mode)
	{
	}

	~Mesh2()
	{
		if( m_vbo )
			glDeleteBuffers(1, &m_vbo);
		if( m_vao )
			glDeleteVertexArrays(1, &m_vao);
	}

	void UpdateData(const size_t count, const V *data)
	{
		m_count = static_cast<GLsizei>(count);
		glNamedBufferData(m_vbo, count * sizeof(V), data, GL_DYNAMIC_DRAW);
	}

	template<size_t Size>
	void UpdateData(const V(&data)[Size])
	{
		UpdateData(Size, data);
	}

	void UpdateData(const std::vector<V> &vertices)
	{
		UpdateData(vertices.size(), vertices.data());
	}

	void BindAndDraw() const
	{
		glBindVertexArray(m_vao);
		glDrawArrays(m_mode, 0, m_count);
	}

protected:
	MoveOnly<GLuint>  m_vbo;
	MoveOnly<GLuint>  m_vao;
	MoveOnly<GLsizei> m_count;
	MoveOnly<GLenum>  m_mode;
};

static void SetupVertexArrayAttrib(
	const GLuint vao,
	const GLuint attribIndex, const GLuint bindingIndex,
	const GLint size, const GLenum type, const GLboolean normalized, const GLuint relativeOffset
)
{
	glEnableVertexArrayAttrib(vao, attribIndex);
	glVertexArrayAttribBinding(vao, attribIndex, bindingIndex);
	glVertexArrayAttribFormat(vao, attribIndex, size, type, normalized, relativeOffset);
}

static void SetupVertexArrayFloatsAttrib(
	const GLuint vao,
	const GLuint attribIndex, const GLuint bindingIndex,
	const GLint size, const GLuint relativeOffset
)
{
	SetupVertexArrayAttrib(vao, attribIndex, bindingIndex, size, GL_FLOAT, GL_FALSE, relativeOffset);
}