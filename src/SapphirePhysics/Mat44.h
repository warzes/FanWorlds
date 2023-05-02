#pragma once

#include "Vec4.h"

// A 4-by-4 matrix stored in column-major order.
class Mat44
{
public:
	Mat44() = default;
	// Set this matrix from four column vectors.
	Mat44(const Vec4& _x, const Vec4& _y, const Vec4& _z, const Vec4& _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	const Vec4& operator[](uint32_t i) const { return (&x)[i]; }
	Vec4& operator[](uint32_t i) { return (&x)[i]; }

	// Read an indexed element from this matrix.
	scalar operator()(uint32_t i, uint32_t j) const
	{
		return (&x.x)[i + 4 * j];
	}

	// Write an indexed element to this matrix.
	scalar& operator()(uint32_t i, uint32_t j)
	{
		return (&x.x)[i + 4 * j];
	}

	// Add a matrix to this matrix.
	void operator+=(const Mat44& B)
	{
		x += B.x;
		y += B.y;
		z += B.z;
		w += B.w;
	}

	// Subtract a matrix from this matrix.
	void operator-=(const Mat44& B)
	{
		x -= B.x;
		y -= B.y;
		z -= B.z;
		w -= B.w;
	}

	// Set this matrix to the zero matrix.
	void SetZero()
	{
		x.SetZero();
		y.SetZero();
		z.SetZero();
		w.SetZero();
	}

	// Set this matrix to the identity matrix.
	void SetIdentity()
	{
		x.Set(scalar(1), scalar(0), scalar(0), scalar(0));
		y.Set(scalar(0), scalar(1), scalar(0), scalar(0));
		z.Set(scalar(0), scalar(0), scalar(1), scalar(0));
		w.Set(scalar(0), scalar(0), scalar(0), scalar(1));
	}

	Vec4 x, y, z, w;
};

// Zero matrix
extern const Mat44 Mat44_zero;

// Identity matrix
extern const Mat44 Mat44_identity;

// Add two matrices.
inline Mat44 operator+(const Mat44& A, const Mat44& B)
{
	return Mat44(A.x + B.x, A.y + B.y, A.z + B.z, A.w + B.w);
}

// Subtract two matrices.
inline Mat44 operator-(const Mat44& A, const Mat44& B)
{
	return Mat44(A.x - B.x, A.y - B.y, A.z - B.z, A.w - B.w);
}

// Multiply a scalar times a matrix.
inline Mat44 operator*(scalar s, const Mat44& A)
{
	return Mat44(s * A.x, s * A.y, s * A.z, s * A.w);
}

// Negate a matrix.
inline Mat44 operator-(const Mat44& A)
{
	return scalar(-1) * A;
}

// Multiply a matrix times a vector.
inline Vec4 operator*(const Mat44& A, const Vec4& v)
{
	return v.x * A.x + v.y * A.y + v.z * A.z + v.w * A.w;
}

// Multiply two matrices.
inline Mat44 operator*(const Mat44& A, const Mat44& B)
{
	return Mat44(A * B.x, A * B.y, A * B.z, A * B.w);
}

// Multiply a matrix times a vector. If the matrix represents a rotation this transforms the vector from one frame to another.
inline Vec4 Mul(const Mat44& A, const Vec4& v)
{
	return v.x * A.x + v.y * A.y + v.z * A.z + v.w * A.w;
}

// Multiply two matrices.
inline Mat44 Mul(const Mat44& A, const Mat44& B)
{
	return Mat44(Mul(A, B.x), Mul(A, B.y), Mul(A, B.z), Mul(A, B.w));
}

// Multiply the transpose of a matrix times a vector.
inline Vec4 MulT(const Mat44& A, const Vec4& v)
{
	return Vec4(Dot(A.x, v), Dot(A.y, v), Dot(A.z, v), Dot(A.w, v));
}

// Multiply the transpose of a matrix times another matrix.
inline Mat44 MulT(const Mat44& A, const Mat44& B)
{
	return Mat44(
		Vec4(Dot(A.x, B.x), Dot(A.y, B.x), Dot(A.z, B.x), Dot(A.w, B.x)),
		Vec4(Dot(A.x, B.y), Dot(A.y, B.y), Dot(A.z, B.y), Dot(A.w, B.y)),
		Vec4(Dot(A.x, B.z), Dot(A.y, B.z), Dot(A.z, B.z), Dot(A.w, B.z)),
		Vec4(Dot(A.x, B.w), Dot(A.y, B.w), Dot(A.z, B.w), Dot(A.w, B.w)));
}

// Return the absolute matrix of a given matrix.
inline Mat44 Abs(const Mat44& A)
{
	return Mat44(Abs(A.x), Abs(A.y), Abs(A.z), Abs(A.w));
}

// Transpose a matrix.
inline Mat44 Transpose(const Mat44& A)
{
	return Mat44(
		Vec4(A.x.x, A.y.x, A.z.x, A.w.x),
		Vec4(A.x.y, A.y.y, A.z.y, A.w.y),
		Vec4(A.x.z, A.y.z, A.z.z, A.w.z),
		Vec4(A.x.w, A.y.w, A.z.w, A.w.w)
	);
}

// Uniform diagonal matrix.
inline Mat44 Mat44Diagonal(scalar s)
{
	return Mat44(
		Vec4(s, scalar(0), scalar(0), scalar(0)),
		Vec4(scalar(0), s, scalar(0), scalar(0)),
		Vec4(scalar(0), scalar(0), s, scalar(0)),
		Vec4(scalar(0), scalar(0), scalar(0), s));
}

// Uniform or non-uniform diagonal matrix.
inline Mat44 Mat44Diagonal(scalar x, scalar y, scalar z, scalar w)
{
	return Mat44(
		Vec4(x, scalar(0), scalar(0), scalar(0)),
		Vec4(scalar(0), y, scalar(0), scalar(0)),
		Vec4(scalar(0), scalar(0), z, scalar(0)),
		Vec4(scalar(0), scalar(0), scalar(0), w));
}

// Invert a matrix.
// If the matrix is singular this returns the zero matrix.
Mat44 Inverse(const Mat44& A);

// Compute the outer product of two vectors.
// The result is a matrix A = a * b^T.
inline Mat44 Outer(const Vec4& a, const Vec4& b)
{
	return Mat44(b.x * a, b.y * a, b.z * a, b.w * a);
}