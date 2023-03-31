#pragma once

#include "MoveOnly.h"

template<class T>
class UniformBuffer
{
public:
	MOVE_ONLY(UniformBuffer);

	UniformBuffer()
	{
		glCreateBuffers(1, &m_buffer);
		glNamedBufferStorage(m_buffer, sizeof(T), nullptr, GL_MAP_WRITE_BIT);
	}

	~UniformBuffer()
	{
		glUnmapNamedBuffer(m_buffer);
		glDeleteBuffers(1, &m_buffer);
	}

	void Bind(const GLuint index)
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, index, m_buffer);
	}

	void Map()
	{
		m_data = static_cast<T *>(glMapNamedBuffer(m_buffer, GL_WRITE_ONLY));
	}

	void Unmap()
	{
		glUnmapNamedBuffer(m_buffer);
	}

	T *operator->()
	{
		return m_data;
	}

private:
	MoveOnly<GLuint> m_buffer;
	MoveOnly<T *> m_data;
};