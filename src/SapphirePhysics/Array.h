#pragma once

// An array for bytes (POD).
template <typename T>
class Array
{
public:
	const T& operator[](uint32_t i) const
	{
		assert(i < m_count);
		return m_elements[i];
	}

	T& operator[](uint32_t i)
	{
		assert(i < m_count);
		return m_elements[i];
	}

	const T* Get(uint32_t i) const
	{
		assert(i < m_count);
		return m_elements + i;
	}

	T* Get(uint32_t i)
	{
		assert(i < m_count);
		return m_elements + i;
	}

	const T* Begin() const
	{
		return m_elements;
	}

	T* Begin()
	{
		return m_elements;
	}

	void PushBack(const T& ele)
	{
		if( m_count == m_capacity )
		{
			T* oldElements = m_elements;
			m_capacity *= 2;
			m_elements = (T*)malloc(m_capacity * sizeof(T));
			memcpy(m_elements, oldElements, m_count * sizeof(T));
			if( oldElements != m_localElements )
			{
				free(oldElements);
			}
		}
		assert(m_count < m_capacity);
		m_elements[m_count] = ele;
		++m_count;
	}

	void PopBack()
	{
		assert(m_count > 0);
		--m_count;
	}

	const T& Back() const
	{
		assert(m_count > 0);
		return m_elements[m_count - 1];
	}

	T& Back()
	{
		assert(m_count > 0);
		return m_elements[m_count - 1];
	}

	uint32_t Capacity() const
	{
		return m_capacity;
	}

	uint32_t Count() const
	{
		return m_count;
	}

	bool IsEmpty() const
	{
		return m_count == 0;
	}

	void Remove(uint32_t index)
	{
		assert(m_count > 0);
		assert(index < m_count);
		--m_count;
		// Swap current element with its next.
		for( uint32_t i = index; i < m_count; ++i )
		{
			m_elements[i] = m_elements[i + 1];
		}
	}

	void Reserve(uint32_t size)
	{
		if( m_capacity < size )
		{
			T* oldElements = m_elements;
			m_capacity = 2 * size;
			m_elements = (T*)malloc(m_capacity * sizeof(T));
			memcpy(m_elements, oldElements, m_count * sizeof(T));
			if( oldElements != m_localElements )
			{
				free(oldElements);
			}
		}

		assert(m_capacity >= size);
	}

	void Resize(uint32_t size)
	{
		if( m_capacity < size )
		{
			T* oldElements = m_elements;
			m_capacity = 2 * size;
			m_elements = (T*)malloc(m_capacity * sizeof(T));
			memcpy(m_elements, oldElements, m_count * sizeof(T));
			if( oldElements != m_localElements )
			{
				free(oldElements);
			}
		}
		assert(m_capacity >= size);
		m_count = size;
	}

	void Swap(const Array<T>& other)
	{
		if( m_elements == other.m_elements )
		{
			return;
		}

		// Ensure sufficient capacity for copy.
		if( m_capacity < other.m_count )
		{
			if( m_elements != m_localElements )
			{
				free(m_elements);
			}
			m_capacity = other.m_capacity;
			m_elements = (T*)malloc(m_capacity * sizeof(T));
		}

		// Copy.
		assert(m_capacity >= other.m_count);
		m_count = other.m_count;
		memcpy(m_elements, other.m_elements, other.m_count * sizeof(T));
	}

	void operator=(const Array<T>& other)
	{
		Swap(other);
	}
protected:
	Array(T* elements, uint32_t N)
	{
		assert(N > 0);
		m_localElements = elements;
		m_capacity = N;
		m_elements = m_localElements;
		m_count = 0;
	}

	Array(const Array<T>& other)
	{
		m_localElements = nullptr;
		m_capacity = 0;
		m_elements = nullptr;
		m_count = 0;

		Swap(other);
	}

	~Array()
	{
		if( m_elements != m_localElements )
		{
			free(m_elements);
		}
	}

	uint32_t m_capacity;
	T* m_elements;
	uint32_t m_count;

	T* m_localElements;
};

template <typename T, uint32_t N>
class StackArray : public Array<T>
{
public:
	StackArray<T, N>() : Array<T>(m_stackElements, N)
	{
	}

	StackArray<T, N>(const StackArray<T, N>& other) : Array<T>(other)
	{
	}

	StackArray<T, N>(const Array<T>& other) : Array<T>(other)
	{
	}

	void operator=(const StackArray<T, N>& other)
	{
		Array<T>::Swap((const Array<T>&)other);
	}

	void operator=(const Array<T>& other)
	{
		Swap(other);
	}

protected:
	T m_stackElements[N];
};