#pragma once

template<typename T>
class MoveOnly
{
public:
	static_assert(std::is_default_constructible_v<T>);
	static_assert(std::is_trivially_copy_constructible_v<T>);
	static_assert(std::is_trivially_copy_assignable_v<T>);
	static_assert(std::is_trivially_move_constructible_v<T>);
	static_assert(std::is_trivially_move_assignable_v<T>);
	static_assert(std::is_swappable_v<T>);

	MoveOnly() = default;

	~MoveOnly() = default;

	explicit MoveOnly(const T &val) noexcept
	{
		m_value = val;
	}

	MoveOnly(const MoveOnly &) = delete;

	MoveOnly &operator=(const MoveOnly &) = delete;

	MoveOnly(MoveOnly &&other) noexcept
	{
		m_value = std::exchange(other.m_value, T{});
	}

	MoveOnly &operator=(MoveOnly &&other) noexcept
	{
		std::swap(m_value, other.m_value);
		return *this;
	}

	operator const T &() const
	{ // NOLINT(google-explicit-constructor)
		return m_value;
	}

	MoveOnly &operator=(const T &val)
	{
		m_value = val;
		return *this;
	}

	// MoveOnly<T> has the same memory layout as T
	// So it's perfectly safe to overload operator&
	T *operator&() // NOLINT(google-runtime-operator)
	{
		return &m_value;
	}

	const T *operator&() const
	{ // NOLINT(google-runtime-operator)
		return &m_value;
	}

	T *operator->()
	{
		return &m_value;
	}

	const T *operator->() const
	{
		return &m_value;
	}

	bool operator==(const T &val) const
	{
		return m_value == val;
	}

	bool operator!=(const T &val) const
	{
		return m_value != val;
	}

private:
	T m_value{};
};

static_assert(sizeof(int) == sizeof(MoveOnly<int>));

#define MOVE_ONLY(T) T(const T&) = delete; T& operator=(const T&) = delete; T(T&&) noexcept = default; T& operator=(T&&) noexcept = default;

#define NO_MOVE_OR_COPY(T) T(const T&) = delete; T& operator=(const T&) = delete; T(T&&) = delete; T& operator=(T&&) = delete;