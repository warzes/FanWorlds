#pragma once

#include "Types.h"

class Vec4
{
public:
	Vec4() = default;
	Vec4(scalar _x, scalar _y, scalar _z, scalar _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	scalar operator[](uint32_t i) const { return (&x)[i]; }
	scalar& operator[](uint32_t i) { return (&x)[i]; }

	// Add a vector to this vector.
	void operator+=(const Vec4& b)
	{
		x += b.x;
		y += b.y;
		z += b.z;
		w += b.w;
	}

	// Subtract a vector from this vector.
	void operator-=(const Vec4& b)
	{
		x -= b.x;
		y -= b.y;
		z -= b.z;
		w -= b.w;
	}

	// Scale this vector.
	void operator*=(scalar s)
	{
		x *= s;
		y *= s;
		z *= s;
		w *= s;
	}

	// Inverse scale this vector.
	void operator/=(scalar a)
	{
		scalar s = scalar(1) / a;
		x *= s;
		y *= s;
		z *= s;
		w *= s;
	}

	// Set this vector to the zero vector.
	void SetZero()
	{
		x = y = z = w = scalar(0);
	}

	// Set this vector from four components.
	void Set(scalar _x, scalar _y, scalar _z, scalar _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	// Convert this vector to the unit vector. Return the length.
	scalar Normalize()
	{
		scalar length = std::sqrt(x * x + y * y + z * z + w * w);
		if (length > SE_EPSILON)
		{
			scalar s = scalar(1) / length;
			x *= s;
			y *= s;
			z *= s;
			w *= s;
		}
		return length;
	}

	scalar x, y, z, w;
};

// Zero vector
extern const Vec4 Vec4_zero;

// Compute the sum of two vectors.
inline Vec4 operator+(const Vec4& a, const Vec4& b)
{
	return Vec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

// Compute the subtraction of two vectors.
inline Vec4 operator-(const Vec4& a, const Vec4& b)
{
	return Vec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

// Compute a scalar-vector product.
inline Vec4 operator*(scalar s, const Vec4& v)
{
	return Vec4(s * v.x, s * v.y, s * v.z, s * v.w);
}

// Compute a scalar-vector product.
inline Vec4 operator*(const Vec4& v, scalar s)
{
	return Vec4(s * v.x, s * v.y, s * v.z, s * v.w);
}

// Inverse multiply a scalar-vector.
inline Vec4 operator/(const Vec4& v, scalar s)
{
	return Vec4(v.x / s, v.y / s, v.z / s, v.w / s);
}

// Compute the dot product of two vectors.
inline scalar Dot(const Vec4& a, const Vec4& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

// Compute the length of a vector.
inline scalar Length(const Vec4& v)
{
	return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

// Compute the squared length of a vector.
inline scalar LengthSquared(const Vec4& v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
}

// Compute the normalized vector of a vector.
inline Vec4 Normalize(const Vec4& v)
{
	scalar length = Length(v);
	if (length > SE_EPSILON)
	{
		scalar s = scalar(1) / length;
		return s * v;
	}
	return v;
}

// Compute the minimum vector between two vectors component-wise.
inline Vec4 Min(const Vec4& a, const Vec4& b)
{
	return Vec4(glm::min(a.x, b.x), glm::min(a.y, b.y), glm::min(a.z, b.z), glm::min(a.w, b.w));
}

// Compute the maximum vector between two vectors component-wise.
inline Vec4 Max(const Vec4& a, const Vec4& b)
{
	return Vec4(glm::max(a.x, b.x), glm::max(a.y, b.y), glm::max(a.z, b.z), glm::max(a.w, b.w));
}

// Compute the absolute vector of a given vector.
inline Vec4 Abs(const Vec4& v)
{
	return Vec4(glm::abs(v.x), glm::abs(v.y), glm::abs(v.z), glm::abs(v.w));
}

// Multiply two vectors component-wise.
inline Vec4 Mul(const Vec4& a, const Vec4& b)
{
	return Vec4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

// Divide two vectors component-wise.
inline Vec4 Div(const Vec4& a, const Vec4& b)
{
	return Vec4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
}