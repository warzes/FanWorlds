#pragma once

// Simple span implementation
template<typename T>
class Span final
{
public:
	constexpr Span() noexcept = default;
	constexpr Span(const T* data, size_t size) noexcept : m_data(data), m_size(size) {}
	template<typename Container>
	constexpr Span(Container& container) noexcept : m_data(container.data()), m_size(container.size()) {}

	constexpr const T* Ddata() const noexcept { return m_data; }
	constexpr size_t Size() const noexcept { return m_size; }

private:
	const T* m_data = nullptr;
	const size_t m_size = 0;
};