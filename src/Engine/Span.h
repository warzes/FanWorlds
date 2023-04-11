#pragma once

template <typename T>
struct Span
{
	Span() : m_begin(nullptr), m_end(nullptr) {}
	Span(T* begin, uint32_t len) : m_begin(begin), m_end(begin + len) {}
	Span(T* begin, T* end) : m_begin(begin), m_end(end) {}
	template <int N> explicit Span(T(&value)[N]) : m_begin(value), m_end(value + N) {}
	T& operator[](uint32_t idx) const { assert(m_begin + idx < m_end); return m_begin[idx]; }
	operator Span<const T>() const { return Span<const T>(m_begin, m_end); }
	[[nodiscard]] Span fromLeft(uint32_t count) const { assert(count <= length()); return Span(m_begin + count, m_end); }
	[[nodiscard]] Span fromRight(uint32_t count) const { assert(count <= length()); return Span(m_begin, m_end - count); }
	T& back() { assert(length() > 0); return *(m_end - 1); }
	const T& back() const { assert(length() > 0); return *(m_end - 1); }
	bool equals(const Span<T>& rhs)
	{
		bool res = true;
		if( length() != rhs.length() ) return false;
		for( const T& v : *this )
		{
			uint32_t i = uint32_t(&v - m_begin);
			if( v != rhs.m_begin[i] ) return false;
		}
		return true;
	}

	template <typename F>
	uint32_t find(const F& f) const
	{
		for( uint32_t i = 0, c = length(); i < c; ++i )
		{
			if( f(m_begin[i]) ) return i;
		}
		return -1;
	}

	uint32_t length() const { return uint32_t(m_end - m_begin); }

	T* begin() const { return m_begin; }
	T* end() const { return m_end; }

	T* m_begin;
	T* m_end;
};