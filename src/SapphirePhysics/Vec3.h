#pragma once

#include "Types.h"

class Vec3
{
public:
	Vec3() = default;
	Vec3(scalar _x, scalar _y, scalar _z) : x(_x), y(_y), z(_z) { }

	scalar operator[](uint32_t i) const { return (&x)[i]; }
	scalar& operator[](uint32_t i) { return (&x)[i]; }

	void operator+=(const Vec3& b) { x += b.x; y += b.y; z += b.z; }
	void operator-=(const Vec3& b) { x -= b.x; y -= b.y; z -= b.z; }
	void operator*=(scalar s) { x *= s; y *= s; z *= s; }
	void operator/=(scalar a) { scalar s = scalar(1) / a; x *= s; y *= s; z *= s; }

	// Set this vector to the zero vector.
	void SetZero()
	{
		x = y = z = scalar(0);
	}

	// Set this vector from three components.
	void Set(scalar _x, scalar _y, scalar _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	scalar Length() const 
	{
		return std::sqrt(LengthSq());
	}

	scalar LengthSq() const 
	{
		return x * x + y * y + z * z;
	}

	// Convert this vector to the unit vector. Return the length.
	void Normalize()
	{
		scalar length_squared = LengthSq();
		if (length_squared > SE_EPSILON)
		{
			scalar s = 1.0f / std::sqrt(length_squared);
			x *= s;
			y *= s;
			z *= s;
		}
	}

	scalar x, y, z;
};

// Zero vector
extern const Vec3 Vec3_zero;
// Right vector
extern const Vec3 Vec3_x;
// Up vector 
extern const Vec3 Vec3_y;
// Forward vector 
extern const Vec3 Vec3_z;

// Negate a vector.
inline Vec3 operator-(const Vec3& v)
{
	return Vec3(-v.x, -v.y, -v.z);
}

// Compute the sum of two vectors.
inline Vec3 operator+(const Vec3& a, const Vec3& b)
{
	return Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

// Compute the subtraction of two vectors.
inline Vec3 operator-(const Vec3& a, const Vec3& b)
{
	return Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

// Compute a scalar-vector product.
inline Vec3 operator*(scalar s, const Vec3& v)
{
	return Vec3(s * v.x, s * v.y, s * v.z);
}

// Compute a scalar-vector product.
inline Vec3 operator*(const Vec3& v, scalar s)
{
	return s * v;
}

// Inverse multiply a scalar-vector.
inline Vec3 operator/(const Vec3& v, scalar s)
{
	scalar a = 1.0f / s;
	return Vec3(v.x * a, v.y * a, v.z * a);
}

// Compute the dot product of two vectors.
inline scalar Dot(const Vec3& a, const Vec3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Compute the cross product of two vectors.
inline Vec3 Cross(const Vec3& a, const Vec3& b)
{
	Vec3 ret;

	const float x2 = a.z * b.y;
	const float y2 = a.x * b.z;
	const float z2 = a.y * b.x;

	ret.x = fmaf(a.y, b.z, -x2);
	ret.y = fmaf(a.z, b.x, -y2);
	ret.z = fmaf(a.x, b.y, -z2);

	return ret;
}

// Compute the determinant of a matrix whose columns are three given vectors.
// Useful property: det(a, b, c) = det(c, a, b) = det(b, c, a).
inline scalar Det(const Vec3& a, const Vec3& b, const Vec3& c)
{
	return Dot(a, Cross(b, c));
}

// Compute the length of a vector.
inline scalar Length(const Vec3& v)
{
	return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

// Compute the squared length of a vector.
inline scalar LengthSquared(const Vec3& v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

// Compute the normalized vector of a vector.
inline Vec3 Normalize(const Vec3& v)
{
	scalar length_squared = LengthSquared(v);
	if (length_squared > SE_EPSILON)
	{
		scalar s = 1.0f / std::sqrt(length_squared);
		return Vec3(
			v.x * s,
			v.y * s,
			v.z * s
		);
	}

	return v;
}

// Compute the euclidean distance between two points.
inline scalar Distance(const Vec3& a, const Vec3& b)
{
	return Length(a - b);
}

// Compute the squared euclidean distance between two points.
inline scalar DistanceSquared(const Vec3& a, const Vec3& b)
{
	return LengthSquared(a - b);
}

// Compute the minimum vector between two vectors component-wise.
inline Vec3 Min(const Vec3& a, const Vec3& b)
{
	return Vec3(glm::min(a.x, b.x), glm::min(a.y, b.y), glm::min(a.z, b.z));
}

// Compute the maximum vector between two vectors component-wise.
inline Vec3 Max(const Vec3& a, const Vec3& b)
{
	return Vec3(glm::max(a.x, b.x), glm::max(a.y, b.y), glm::max(a.z, b.z));
}

// Compute the absolute vector of a given vector.
inline Vec3 Abs(const Vec3& v)
{
	return Vec3(glm::abs(v.x), glm::abs(v.y), glm::abs(v.z));
}

// Multiply two vectors component-wise.
inline Vec3 Mul(const Vec3& a, const Vec3& b)
{
	return Vec3(a.x * b.x, a.y * b.y, a.z * b.z);
}

// Divide two vectors component-wise.
inline Vec3 Div(const Vec3& a, const Vec3& b)
{
	return Vec3(a.x / b.x, a.y / b.y, a.z / b.z);
}

// Compute a perpendicular unit vector to a given unit vector.
inline Vec3 Perp(const Vec3& v)
{
	static const scalar sqrt_inv3 = std::sqrt(scalar(1) / scalar(3));

	Vec3 u;
	if (std::abs(v.x) >= sqrt_inv3)
	{
		u.Set(v.y, -v.x, scalar(0));
	}
	else
	{
		u.Set(scalar(0), v.z, -v.y);
	}

	u.Normalize();
	return u;
}