#pragma once


// A growable LIFO stack with an initial capacity of N.
// If the stack capacity exceeds the initial capacity, the heap 
// is used to increase the capacity of the stack.
template <typename T, uint32_t N>
class Stack
{
public:
	Stack()
	{
		m_capacity = N;
		m_elements = m_stackElements;
		m_count = 0;
	}

	~Stack()
	{
		if( m_elements != m_stackElements )
		{
			free(m_elements);
		}
		m_elements = nullptr;
	}

	const T& Top() const
	{
		assert(m_count);
		return m_elements[m_count - 1];
	}

	T& Top()
	{
		assert(m_count);
		return m_elements[m_count - 1];
	}

	void Push(const T& ele)
	{
		if( m_count == m_capacity )
		{
			T* oldElements = m_elements;
			m_capacity *= 2;
			m_elements = (T*)malloc(m_capacity * sizeof(T));
			memcpy(m_elements, oldElements, m_count * sizeof(T));
			if( oldElements != m_stackElements )
			{
				free(oldElements);
			}
		}
		assert(m_count < m_capacity);
		m_elements[m_count] = ele;
		++m_count;
	}

	void Pop()
	{
		assert(m_count);
		--m_count;
	}

	uint32_t Count() const
	{
		return m_count;
	}

	bool IsEmpty() const
	{
		return m_count == 0;
	}

	void Clear()
	{
		m_count = 0;
	}
private:
	uint32_t m_capacity;
	T* m_elements;
	uint32_t m_count;
	T m_stackElements[N];
};