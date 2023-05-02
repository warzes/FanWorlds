#pragma once

#include "Vec3.h"

// A 3-by-3 matrix stored in column-major order.
class Mat33
{
public:
	// Does nothing for performance.
	Mat33() = default;

	// Set this matrix from three column vectors.
	Mat33(const Vec3& _x, const Vec3& _y, const Vec3& _z) : x(_x), y(_y), z(_z) { }

	const Vec3& operator[](uint32_t i) const { return (&x)[i]; }
	Vec3& operator[](uint32_t i) { return (&x)[i]; }

	scalar operator()(uint32_t i, uint32_t j) const
	{
		return (&x.x)[i + 3 * j];
	}
	scalar& operator()(uint32_t i, uint32_t j)
	{
		return (&x.x)[i + 3 * j];
	}

	// Add a matrix to this matrix.
	void operator+=(const Mat33& B)
	{
		x += B.x;
		y += B.y;
		z += B.z;
	}

	// Subtract a matrix from this matrix.
	void operator-=(const Mat33& B)
	{
		x -= B.x;
		y -= B.y;
		z -= B.z;
	}

	// Set this matrix to the zero matrix.
	void SetZero()
	{
		x.SetZero();
		y.SetZero();
		z.SetZero();
	}

	// Set this matrix to the identity matrix.
	void SetIdentity()
	{
		x.Set(scalar(1), scalar(0), scalar(0));
		y.Set(scalar(0), scalar(1), scalar(0));
		z.Set(scalar(0), scalar(0), scalar(1));
	}

	// Solve Ax = b. 
	// It doesn't compute the inverse. 
	// Therefore, is more efficient.
	// Returns the zero vector if the matrix is singular.
	Vec3 Solve(const Vec3& b) const;

	Vec3 x, y, z;
};

// Zero matrix
extern const Mat33 Mat33_zero;

// Identity matrix
extern const Mat33 Mat33_identity;

// Add two matrices.
inline Mat33 operator+(const Mat33& A, const Mat33& B)
{
	return Mat33(A.x + B.x, A.y + B.y, A.z + B.z);
}

// Subtract two matrices.
inline Mat33 operator-(const Mat33& A, const Mat33& B)
{
	return Mat33(A.x - B.x, A.y - B.y, A.z - B.z);
}

// Multiply a scalar times a matrix.
inline Mat33 operator*(scalar s, const Mat33& A)
{
	return Mat33(s * A.x, s * A.y, s * A.z);
}

// Negate a matrix.
inline Mat33 operator-(const Mat33& A)
{
	return scalar(-1) * A;
}

// Multiply a matrix times a vector.
inline Vec3 operator*(const Mat33& A, const Vec3& v)
{
	return v.x * A.x + v.y * A.y + v.z * A.z;
}

// Multiply two matrices.
inline Mat33 operator*(const Mat33& A, const Mat33& B)
{
	return Mat33(A * B.x, A * B.y, A * B.z);
}

// Multiply a matrix times a vector. If the matrix represents a rotation this transforms the vector from one frame to another.
inline Vec3 Mul(const Mat33& A, const Vec3& v)
{
	return v.x * A.x + v.y * A.y + v.z * A.z;
}

// Multiply two matrices.
inline Mat33 Mul(const Mat33& A, const Mat33& B)
{
	return Mat33(Mul(A, B.x), Mul(A, B.y), Mul(A, B.z));
}

// Multiply the transpose of a matrix times a vector.
inline Vec3 MulT(const Mat33& A, const Vec3& v)
{
	return Vec3(Dot(A.x, v), Dot(A.y, v), Dot(A.z, v));
}

// Multiply the transpose of a matrix times another matrix.
inline Mat33 MulT(const Mat33& A, const Mat33& B)
{
	return Mat33(
		Vec3(Dot(A.x, B.x), Dot(A.y, B.x), Dot(A.z, B.x)),
		Vec3(Dot(A.x, B.y), Dot(A.y, B.y), Dot(A.z, B.y)),
		Vec3(Dot(A.x, B.z), Dot(A.y, B.z), Dot(A.z, B.z)));
}

// Return the absolute matrix of a given matrix.
inline Mat33 Abs(const Mat33& A)
{
	return Mat33(Abs(A.x), Abs(A.y), Abs(A.z));
}

// Transpose a matrix.
inline Mat33 Transpose(const Mat33& A)
{
	return Mat33(
		Vec3(A.x.x, A.y.x, A.z.x),
		Vec3(A.x.y, A.y.y, A.z.y),
		Vec3(A.x.z, A.y.z, A.z.z)
	);
}

// Uniform diagonal matrix.
inline Mat33 Mat33Diagonal(scalar s)
{
	return Mat33(
		Vec3(s, scalar(0), scalar(0)),
		Vec3(scalar(0), s, scalar(0)),
		Vec3(scalar(0), scalar(0), s));
}

// Uniform or non-uniform diagonal matrix.
inline Mat33 Mat33Diagonal(scalar x, scalar y, scalar z)
{
	return Mat33(
		Vec3(x, scalar(0), scalar(0)),
		Vec3(scalar(0), y, scalar(0)),
		Vec3(scalar(0), scalar(0), z));
}

// Return the cofactor matrix of a given matrix.
inline Mat33 CofactorMatrix(const Mat33& A)
{
	// Cofactor matrix using cross products.
	//
	// c = cross(a, b) = [i  j  k ] = i * minor(i) - j * minor(j) + k * minor(k)
	//                   [a1 a2 a3]
	//                   [b1 b2 b3]
	//
	// Therefore,
	//
	// c1 = minor(i)
	// c2 = -minor(j)
	// c3 = minor(k)
	//
	// A = [a11 a12 a13]
	//     [a21 a22 a23]
	//     [a31 a32 a33]
	// 
	// Properties used:
	// 
	// |A| = |A^T|
	// 
	// |a b| = -|c a|
	// |c d|    |d b| 
	// 
	// cx  = cross(y, z) = [i   j   k  ]
	//                     [a12 a22 a32]
	//                     [a13 a23 a33]
	// 
	// c1 = |a22 a32| = |a22 a23|
	//      |a23 a33|   |a32 a33|
	// 
	// c2 = -|a12 a32| = -|a12 a13|
	//       |a13 a33|    |a32 a33|
	// 
	// c3 = |a12 a22| = |a12 a13|
	//      |a13 a23|   |a22 a23|
	//
	// cy = cross(z, x) = [i   j   k  ]
	//                    [a13 a23 a33]
	//                    [a11 a21 a31]
	// 
	// c1 = |a23 a33| = -|a21 a23| 
	//      |a21 a31|    |a31 a33|
	//
	// c2 = -|a13 a33| = -(-|a11 a13|) = |a11 a13|
	//       |a11 a31|	  ( |a31 a33|)   |a31 a33|
	// 
	// c2 = |a13 a23| = -|a11 a13|
	//      |a11 a21|    |a21 a23|
	// 
	// cz = cross(x, y) = [i   j   k  ]
	//                    [a11 a21 a31]
	//                    [a12 a22 a32]
	// 
	// c1 = |a21 a31| = |a21 a22|
	//      |a22 a32|   |a31 a32|
	//
	// c2 = -|a11 a31| = -|a11 a12|
	//       |a12 a32|    |a31 a32|
	//
	// c3 = |a11 a21| = |a11 a12|
	//      |a12 a22|   |a21 a22|
	//
	Mat33 C;
	C.x = Cross(A.y, A.z);
	C.y = Cross(A.z, A.x);
	C.z = Cross(A.x, A.y);
	return C;
}

// Return the adjugate of a matrix.
inline Mat33 Adjugate(const Mat33& A)
{
	return Transpose(CofactorMatrix(A));
}

// Invert a matrix.
// If the matrix is singular this returns the zero matrix.
Mat33 Inverse(const Mat33& A);

// Invert a symmetric matrix.
// If the matrix is singular this returns the zero matrix.
Mat33 SymInverse(const Mat33& A);

// Return a skew (anti-symmetric) matrix for a vector such that skew(v) * w = cross(v, w).
inline Mat33 Skew(const Vec3& v)
{
	return Mat33(
		Vec3(scalar(0), v.z, -v.y),
		Vec3(-v.z, scalar(0), v.x),
		Vec3(v.y, -v.x, scalar(0)));
}

// Compute the outer product of two vectors.
// The result is a matrix A = a * b^T.
inline Mat33 Outer(const Vec3& a, const Vec3& b)
{
	return Mat33(b.x * a, b.y * a, b.z * a);
}

// Compute an orthogonal basis given one of its vectors.
// The vector must be normalized.
inline void ComputeBasis(const Vec3& a, Vec3& b, Vec3& c)
{
	// https://box2d.org/2014/02/computing-a-basis/, (Erin)
	// Suppose vector a has all equal components and is a unit vector : a = (s, s, s)
	// Then 3*s*s = 1, s = sqrt(1/3). 
	// This means that at least one component of a unit vector must be greater or equal to s.
	static const scalar sqrt_inv3 = std::sqrt(scalar(1) / scalar(3));

	if (std::abs(a.x) >= sqrt_inv3)
	{
		b.Set(a.y, -a.x, scalar(0));
	}
	else
	{
		b.Set(scalar(0), a.z, -a.y);
	}

	b.Normalize();
	c = Cross(a, b);
}