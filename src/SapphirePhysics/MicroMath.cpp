#include "stdafx.h"
#include "MicroMath.h"
#include "Mat22.h"
#include "Mat33.h"
#include "Mat44.h"
#include "Transform.h"

const Vec3 Vec3_zero(scalar(0), scalar(0), scalar(0));
const Vec3 Vec3_x(scalar(1), scalar(0), scalar(0));
const Vec3 Vec3_y(scalar(0), scalar(1), scalar(0));
const Vec3 Vec3_z(scalar(0), scalar(0), scalar(1));

const Vec4 Vec4_zero(scalar(0), scalar(0), scalar(0), scalar(0));

const Mat33 Mat33_zero(
	Vec3(scalar(0), scalar(0), scalar(0)),
	Vec3(scalar(0), scalar(0), scalar(0)),
	Vec3(scalar(0), scalar(0), scalar(0)));

const Mat33 Mat33_identity(
	Vec3(scalar(1), scalar(0), scalar(0)),
	Vec3(scalar(0), scalar(1), scalar(0)),
	Vec3(scalar(0), scalar(0), scalar(1)));

const Mat44 Mat44_zero(
	Vec4(scalar(0), scalar(0), scalar(0), scalar(0)),
	Vec4(scalar(0), scalar(0), scalar(0), scalar(0)),
	Vec4(scalar(0), scalar(0), scalar(0), scalar(0)),
	Vec4(scalar(0), scalar(0), scalar(0), scalar(0)));

const Mat44 Mat44_identity(
	Vec4(scalar(1), scalar(0), scalar(0), scalar(0)),
	Vec4(scalar(0), scalar(1), scalar(0), scalar(0)),
	Vec4(scalar(0), scalar(0), scalar(1), scalar(0)),
	Vec4(scalar(0), scalar(0), scalar(0), scalar(1)));

const Transform Transform_identity(Vec3_zero, Quat_identity);

const Quat Quat_identity(scalar(0), scalar(0), scalar(0), scalar(1));

Vec3 Mat33::Solve(const Vec3& b) const
{
	// Cramer's rule
	scalar det = Det(x, y, z);
	if (det != scalar(0))
	{
		det = scalar(1) / det;
	}

	Vec3 xn;
	xn.x = det * Det(b, y, z);
	xn.y = det * Det(x, b, z);
	xn.z = det * Det(x, y, b);
	return xn;
}

Mat33 Inverse(const Mat33& A)
{
	// Cofactor method
	scalar det = Det(A.x, A.y, A.z);
	if (det != scalar(0))
	{
		det = scalar(1) / det;
	}
	return det * Adjugate(A);
}

Mat33 SymInverse(const Mat33& A)
{
	scalar det = Det(A.x, A.y, A.z);
	if (det != scalar(0))
	{
		det = scalar(1) / det;
	}

	scalar a11 = A.x.x, a12 = A.y.x, a13 = A.z.x;
	scalar a22 = A.y.y, a23 = A.z.y;
	scalar a33 = A.z.z;

	Mat33 M;

	M.x.x = det * (a22 * a33 - a23 * a23);
	M.x.y = det * (a13 * a23 - a12 * a33);
	M.x.z = det * (a12 * a23 - a13 * a22);

	M.y.x = M.x.y;
	M.y.y = det * (a11 * a33 - a13 * a13);
	M.y.z = det * (a13 * a12 - a11 * a23);

	M.z.x = M.x.z;
	M.z.y = M.y.z;
	M.z.z = det * (a11 * a22 - a12 * a12);

	return M;
}

static SE_FORCE_INLINE scalar Minor(const Mat44& A, uint32_t i, uint32_t j)
{
	Mat33 S;
	SubMatrix(&S.x.x, &A.x.x, 4, 4, i, j);
	return Det(S.x, S.y, S.z);
}

// (-1)^(i + j)
static SE_FORCE_INLINE int32_t CofactorSign(uint32_t i, uint32_t j)
{
	return (i + j) % 2 == 0 ? 1 : -1;
}

static SE_FORCE_INLINE scalar Cofactor(const Mat44& A, uint32_t i, uint32_t j)
{
	int32_t sign = CofactorSign(i, j);
	scalar minor = Minor(A, i, j);
	return scalar(sign) * minor;
}

static SE_FORCE_INLINE Mat44 CofactorMatrix(const Mat44& A)
{
	Mat44 C;
	for (uint32_t i = 0; i < 4; ++i)
	{
		for (uint32_t j = 0; j < 4; ++j)
		{
			C(i, j) = Cofactor(A, i, j);
		}
	}
	return C;
}

static SE_FORCE_INLINE scalar Det(const Mat44& A, const Mat44& C)
{
	// Cofactor expansion along the first row
	scalar result = scalar(0);
	for (uint32_t j = 0; j < 4; ++j)
	{
		result += A(0, j) * C(0, j);
	}
	return result;
}

Mat44 Inverse(const Mat44& A)
{
	// Cofactor matrix
	Mat44 C = CofactorMatrix(A);

	scalar det = Det(A, C);

	if (det != scalar(0))
	{
		det = scalar(1) / det;
	}

	// A^-1 = |A|^-1 * adj(A) = |A|^-1 * C^T
	return det * Transpose(C);
}