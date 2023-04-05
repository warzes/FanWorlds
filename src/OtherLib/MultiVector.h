#pragma once

// зависит от mdspan от Kokkos - удалить когда придет новый стандарт (оно войдет в стандарт)

/*
* https://github.com/acdemiralp/multi
		MultiVector<float, 3> multi_vector({ 4, 4, 4 }, 0.0f);

		// Resize.
		multi_vector.resize({ 8, 8, 8 });

		// Access.
		for( std::size_t x = 0; x < multi_vector.dimensions()[0]; ++x )
			for( std::size_t y = 0; y < multi_vector.dimensions()[1]; ++y )
				for( std::size_t z = 0; z < multi_vector.dimensions()[2]; ++z )
					multi_vector(x, y, z) = static_cast<float>(x + y + z);

		// Linear access.
		for( std::size_t i = 0; i < multi_vector.size(); ++i )
			multi_vector[i] = static_cast<float>(i);

		// Linear iteration.
		std::for_each(multi_vector.begin(), multi_vector.end(), [](auto& value) { value += 42.0f; });
*/

template <
	typename    _type,
	std::size_t _dimensions,
	typename    _layout = std::experimental::layout_right,
	typename    _accessor = std::experimental::default_accessor<_type>,
	typename    _allocator = std::allocator<_type>>
	class MultiVector
{
public:
	using storage_type = std::vector<_type, _allocator>;
	using span_type = std::experimental::mdspan<_type, std::experimental::dextents<_dimensions>, _layout, _accessor>;

	using value_type = typename storage_type::value_type;
	using allocator_type = typename storage_type::allocator_type;
	using size_type = typename storage_type::size_type;
	using difference_type = typename storage_type::difference_type;
	using reference = typename storage_type::reference;
	using const_reference = typename storage_type::const_reference;
	using pointer = typename storage_type::pointer;
	using const_pointer = typename storage_type::const_pointer;
	using iterator = typename storage_type::iterator;
	using const_iterator = typename storage_type::const_iterator;
	using reverse_iterator = typename storage_type::reverse_iterator;
	using const_reverse_iterator = typename storage_type::const_reverse_iterator;

	using multi_size_type = std::array<size_type, _dimensions>;

	constexpr          MultiVector() noexcept(noexcept(allocator_type())) = default;
	constexpr explicit MultiVector(const allocator_type& alloc) noexcept
		: storage_(alloc)
	{

	}

	template <                          size_type _d = _dimensions, typename std::enable_if_t<_d == 1, size_type> = 0>
	constexpr          MultiVector(size_type size, const_reference value = value_type(), const allocator_type& alloc = allocator_type())
		: storage_(size, value, alloc), span_(storage_.data(), storage_.size())
	{

	}
	template <                          size_type _d = _dimensions, typename std::enable_if_t<_d == 1, size_type> = 0>
	constexpr explicit MultiVector(size_type size, const allocator_type& alloc = allocator_type())
		: storage_(size, alloc), span_(storage_.data(), storage_.size())
	{

	}
	template <typename _input_iterator, size_type _d = _dimensions, typename std::enable_if_t<_d == 1, size_type> = 0>
	constexpr          MultiVector(_input_iterator first, _input_iterator last, const allocator_type& alloc = allocator_type())
		: storage_(first, last, alloc), span_(storage_.data(), storage_.size())
	{

	}
	template <                          size_type _d = _dimensions, typename std::enable_if_t<_d == 1, size_type> = 0>
	constexpr          MultiVector(std::initializer_list<value_type> list, const allocator_type& alloc = allocator_type())
		: storage_(list, alloc), span_(storage_.data(), storage_.size())
	{

	}

	template <                          size_type _d = _dimensions, typename std::enable_if_t<(_d > 1), size_type> = 0>
		constexpr          MultiVector(const multi_size_type& size, const_reference value = value_type(), const allocator_type& alloc = allocator_type())
		: storage_(linear_size(size), value, alloc), span_(storage_.data(), size)
	{

	}
	template <                          size_type _d = _dimensions, typename std::enable_if_t<(_d > 1), size_type> = 0>
constexpr explicit MultiVector(const multi_size_type& size, const allocator_type& alloc = allocator_type())
	: storage_(linear_size(size), alloc), span_(storage_.data(), size)
{

}
template <typename _input_iterator, size_type _d = _dimensions, typename std::enable_if_t<(_d > 1), size_type> = 0>
constexpr          MultiVector(const multi_size_type& size, _input_iterator first, _input_iterator last, const allocator_type& alloc = allocator_type())
	: storage_(linear_size(size), alloc), span_(storage_.data(), size)
{
	std::copy(first, last, storage_.begin());
}
template <                          size_type _d = _dimensions, typename std::enable_if_t<(_d > 1), size_type> = 0>
constexpr          MultiVector(const multi_size_type& size, std::initializer_list<value_type> list, const allocator_type& alloc = allocator_type())
	: storage_(linear_size(size), alloc), span_(storage_.data(), size)
{
	std::copy(list.begin(), list.end(), storage_.begin());
}

constexpr          MultiVector(const MultiVector&  that)
	: storage_(that.storage_), span_(storage_.data(), that.span_.mapping(), that.span_.accessor())
{

}
constexpr          MultiVector(const MultiVector&  that, const allocator_type& alloc)
	: storage_(that.storage_, alloc), span_(storage_.data(), that.span_.mapping(), that.span_.accessor())
{

}
constexpr          MultiVector(MultiVector&& temp) noexcept
	: storage_(std::move(temp.storage_)), span_(storage_.data(), temp.span_.mapping(), temp.span_.accessor())
{
	temp.storage_ = {};
	temp.span_ = {};
}
constexpr          MultiVector(MultiVector&& temp, const allocator_type& alloc)
	: storage_(std::move(temp.storage_), alloc), span_(storage_.data(), temp.span_.mapping(), temp.span_.accessor())
{
	temp.storage_ = {};
	temp.span_ = {};
}

constexpr         ~MultiVector() = default;

constexpr MultiVector&                operator=    (const MultiVector&  that)
{
	if( this != &that )
	{
		storage_ = that.storage_;
		span_ = span_type(storage_.data(), that.span_.mapping(), that.span_.accessor());
	}
	return *this;
}
constexpr MultiVector&                operator=    (MultiVector&& temp) noexcept
{
	if( this != &temp )
	{
		storage_ = std::move(temp.storage_);
		span_ = span_type(storage_.data(), temp.span_.mapping(), temp.span_.accessor());

		temp.storage_ = {};
		temp.span_ = {};
	}
	return *this;
}

template <size_type _d = _dimensions, typename = std::enable_if_t<_d == 1>>
constexpr MultiVector&                operator=    (std::initializer_list<_type> list)
{
	storage_ = list;
	span_ = span_type(storage_.data(), storage_.size());
	return *this;
}

template <                          size_type _d = _dimensions, typename = std::enable_if_t<_d == 1>>
constexpr void                   assign(size_type size, const_reference value)
{
	storage_.assign(size, value);
	span_ = span_type(storage_.data(), size);
}
template <typename _input_iterator, size_type _d = _dimensions, typename = std::enable_if_t<_d == 1>>
constexpr void                   assign(_input_iterator first, _input_iterator last)
{
	storage_.assign(first, last);
	span_ = span_type(storage_.data(), storage_.size());
}
template <                          size_type _d = _dimensions, typename = std::enable_if_t<_d == 1>>
constexpr void                   assign(std::initializer_list<_type> list)
{
	storage_.assign(list);
	span_ = span_type(storage_.data(), storage_.size());
}

template <                          size_type _d = _dimensions, typename = std::enable_if_t<(_d > 1)>>
constexpr void                   assign(const multi_size_type& size, const_reference value)
{
	storage_.assign(linear_size(size), value);
	span_ = span_type(storage_.data(), size);
}
template <typename _input_iterator, size_type _d = _dimensions, typename = std::enable_if_t<(_d > 1)>>
constexpr void                   assign(const multi_size_type& size, _input_iterator first, _input_iterator last)
{
	storage_.resize(linear_size(size));
	std::copy(first, last, storage_.begin());
	span_ = span_type(storage_.data(), size);
}
template <                          size_type _d = _dimensions, typename = std::enable_if_t<(_d > 1)>>
constexpr void                   assign(const multi_size_type& size, std::initializer_list<_type> list)
{
	storage_.resize(linear_size(size));
	std::copy(list.begin(), list.end(), storage_.begin());
	span_ = span_type(storage_.data(), size);
}

constexpr allocator_type         get_allocator() const noexcept
{
	return storage_.get_allocator();
}

// Element access.

constexpr reference              at(size_type              position)
{
	return storage_.at(position);
}
constexpr const_reference        at(size_type              position) const
{
	return storage_.at(position);
}
constexpr reference              at(const multi_size_type& position)
{
	return span_(position);
}
constexpr const_reference        at(const multi_size_type& position) const
{
	return span_(position);
}
template <typename... _positions>
constexpr reference              at(_positions...          position)
{
	return span_(position...);
}
template <typename... _positions>
constexpr const_reference        at(_positions...          position) const
{
	return span_(position...);
}

constexpr reference              operator[]   (size_type              position)
{
	return storage_[position];
}
constexpr const_reference        operator[]   (size_type              position) const
{
	return storage_[position];
}
constexpr reference              operator[]   (const multi_size_type& position)
{
	return span_(position);
}
constexpr const_reference        operator[]   (const multi_size_type& position) const
{
	return span_(position);
}

constexpr reference              operator()   (const multi_size_type& position)
{
	return span_(position);
}
constexpr const_reference        operator()   (const multi_size_type& position) const
{
	return span_(position);
}
template <typename... _positions>
constexpr reference              operator()   (_positions...          position)
{
	return span_(position...);
}
template <typename... _positions>
constexpr const_reference        operator()   (_positions...          position) const
{
	return span_(position...);
}

constexpr reference              front()
{
	return storage_.front();
}
constexpr const_reference        front() const
{
	return storage_.front();
}

constexpr reference              back()
{
	return storage_.back();
}
constexpr const_reference        back() const
{
	return storage_.back();
}

constexpr pointer                data() noexcept
{
	return storage_.data();
}
constexpr const_pointer          data() const noexcept
{
	return storage_.data();
}

// Iterators.

constexpr iterator               begin() noexcept
{
	return storage_.begin();
}
constexpr const_iterator         begin() const noexcept
{
	return storage_.begin();
}
constexpr const_iterator         cbegin() const noexcept
{
	return storage_.cbegin();
}

constexpr iterator               end() noexcept
{
	return storage_.end();
}
constexpr const_iterator         end() const noexcept
{
	return storage_.end();
}
constexpr const_iterator         cend() const noexcept
{
	return storage_.cend();
}

constexpr reverse_iterator       rbegin() noexcept
{
	return storage_.rbegin();
}
constexpr const_reverse_iterator rbegin() const noexcept
{
	return storage_.rbegin();
}
constexpr const_reverse_iterator crbegin() const noexcept
{
	return storage_.crbegin();
}

constexpr reverse_iterator       rend() noexcept
{
	return storage_.rend();
}
constexpr const_reverse_iterator rend() const noexcept
{
	return storage_.rend();
}
constexpr const_reverse_iterator crend() const noexcept
{
	return storage_.crend();
}

// Capacity.

constexpr bool                   empty() const noexcept
{
	return storage_.empty();
}
constexpr size_type              size() const noexcept
{
	return storage_.size();
}
constexpr multi_size_type        dimensions() const noexcept
{
	multi_size_type result;
	for( size_type i = 0; i < span_type::rank(); ++i )
		result[i] = span_.extent(i);
	return result;
}
constexpr size_type              max_size() const noexcept
{
	return storage_.max_size();
}
constexpr void                   reserve(size_type capacity)
{
	storage_.reserve(capacity);
}
constexpr size_type              capacity() const noexcept
{
	return storage_.capacity();
}
constexpr void                   shrink_to_fit()
{
	storage_.shrink_to_fit();
}

// Modifiers (hyper-rectangular storages have no correspondents for insert, emplace, erase, push_back, emplace_back, pop_back).

constexpr void                   clear() noexcept
{
	storage_.clear();
	span_ = {};
}

template <size_type _d = _dimensions, typename = std::enable_if_t<_d == 1>>
constexpr void                   resize(size_type          size)
{
	storage_.resize(size);
	span_ = span_type(storage_.data(), size);
}
template <size_type _d = _dimensions, typename = std::enable_if_t<_d == 1>>
constexpr void                   resize(size_type          size, const_reference value)
{
	storage_.resize(size, value);
	span_ = span_type(storage_.data(), size);
}

template <size_type _d = _dimensions, typename = std::enable_if_t<(_d > 1)>>
constexpr void                   resize(const multi_size_type& size)
{
	storage_.resize(linear_size(size));
	span_ = span_type(storage_.data(), size);
}
template <size_type _d = _dimensions, typename = std::enable_if_t<(_d > 1)>>
constexpr void                   resize(const multi_size_type& size, const_reference value)
{
	storage_.resize(linear_size(size), value);
	span_ = span_type(storage_.data(), size);
}

constexpr void                   swap(MultiVector& that) noexcept
{
	std::swap(storage_, that.storage_);
	std::swap(span_, that.span_);
}

// Member access.

constexpr const storage_type&    storage() const noexcept
{
	return storage_;
}
constexpr const span_type&       span() const noexcept
{
	return span_;
}

protected:
	static constexpr size_type       linear_size(const multi_size_type& size)
	{
		return std::accumulate(size.begin(), size.end(), static_cast<size_type>(1), std::multiplies<size_type>());
	}

	storage_type storage_;
	span_type    span_;
};

// Non-member functions (hyper-rectangular storages have no correspondents for erase, erase_if).

template <typename _type, std::size_t _dimensions, typename _layout, typename _accessor, typename _allocator>
constexpr bool operator== (
	const MultiVector<_type, _dimensions, _layout, _accessor, _allocator>& lhs,
	const MultiVector<_type, _dimensions, _layout, _accessor, _allocator>& rhs)
{
	return lhs.storage() == rhs.storage() && lhs.span().mapping() == rhs.span().mapping();
}
template <typename _type, std::size_t _dimensions, typename _layout, typename _accessor, typename _allocator>
constexpr auto operator<=>(
	const MultiVector<_type, _dimensions, _layout, _accessor, _allocator>& lhs,
	const MultiVector<_type, _dimensions, _layout, _accessor, _allocator>& rhs)
{
	return lhs.storage() <=> rhs.storage();
}

template <typename _type, std::size_t _dimensions, typename _layout, typename _accessor, typename _allocator>
constexpr void swap(
	MultiVector<_type, _dimensions, _layout, _accessor, _allocator>& lhs,
	MultiVector<_type, _dimensions, _layout, _accessor, _allocator>& rhs) noexcept
{
	lhs.swap(rhs);
}