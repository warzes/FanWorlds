#pragma once

#include "Types.h"

template <class T>
inline T Sign(T x)
{
	return T(T(0) < x) - T(T(x) < T(0));
}

template <class T>
inline uint32_t UniqueCount(const T* V, uint32_t N)
{
	uint32_t count = 0;
	for (uint32_t i = 0; i < N; ++i)
	{
		uint32_t j;
		for (j = 0; j < N; ++j)
		{
			if (V[i] == V[j])
			{
				break;
			}
		}
		if (i == j)
		{
			++count;
		}
	}
	return count;
}

// Multiply two matrices stored in column-major order.
// C = A * B
inline void Mul(scalar* C, const scalar* A, uint32_t AM, uint32_t AN, const scalar* B, uint32_t BM, uint32_t BN)
{
	assert(AN == BM);

	for (uint32_t i = 0; i < AM; ++i)
	{
		for (uint32_t j = 0; j < BN; ++j)
		{
			C[i + AM * j] = scalar(0);

			for (uint32_t k = 0; k < AN; ++k)
			{
				C[i + AM * j] += A[i + AM * k] * B[k + BM * j];
			}
		}
	}
}

// Return the transpose of a given matrix stored in column-major order.
// B = A^T
inline void Transpose(scalar* B, const scalar* A, uint32_t AM, uint32_t AN)
{
	for (uint32_t i = 0; i < AM; ++i)
	{
		for (uint32_t j = 0; j < AN; ++j)
		{
			B[j + AN * i] = A[i + AM * j];
		}
	}
}

// Return the original matrix with the given row and column ignored.
// The row and column parameters must be zero based indices to the element.
// The given matrix must be stored in column-major order.
inline void SubMatrix(scalar* out, const scalar* A, uint32_t AM, uint32_t AN, uint32_t row, uint32_t col)
{
	assert(row < AM);
	assert(col < AN);

	uint32_t count = 0;

	for (uint32_t j = 0; j < AN; ++j)
	{
		for (uint32_t i = 0; i < AM; ++i)
		{
			if (i != row && j != col)
			{
				out[count++] = A[i + AM * j];
			}
		}
	}
}

// Return the lenght of a given vector.
// ||v||
inline scalar Length(const scalar* v, uint32_t n)
{
	scalar result(0);
	for (uint32_t i = 0; i < n; ++i)
	{
		result += v[i] * v[i];
	}
	return std::sqrt(result);
}