/*
* Copyright (c) 2016-2019 Irlan Robson https://irlanrobson.github.io
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef B3_SPARSE_MAT_33_H
#define B3_SPARSE_MAT_33_H

#include <common/math/mat33.h>
#include <sparse/diag_mat33.h>
#include <sparse/dense_vec3.h>

// An element in a sparse matrix.
struct b3RowValue
{
	u32 column;
	b3Mat33 value;
	b3RowValue* next;
};

// Singly linked list of row elements.
struct b3RowValueList
{
	b3RowValueList()
	{
		head = nullptr;
		count = 0;
	}

	~b3RowValueList() { }

	void PushFront(b3RowValue* link)
	{
		link->next = head;
		head = link;
		++count;
	}

	b3RowValue* head;
	u32 count;
};

// A sparse matrix.
// Each row is a list of non-zero elements in the row.
struct b3SparseMat33
{
	//
	b3SparseMat33();

	// 
	b3SparseMat33(u32 m);

	//
	b3SparseMat33(const b3SparseMat33& _m);

	//
	~b3SparseMat33();

	//
	b3SparseMat33& operator=(const b3SparseMat33& _m);

	// 
	void Copy(const b3SparseMat33& _m);

	//
	void Destroy();

	// 
	b3Mat33& operator()(u32 i, u32 j);

	// 
	const b3Mat33& operator()(u32 i, u32 j) const;

	// 
	void operator+=(const b3SparseMat33& m);

	// 
	void operator-=(const b3SparseMat33& m);

	u32 rowCount;
	b3RowValueList* rows;
};

inline b3SparseMat33::b3SparseMat33()
{
	rowCount = 0;
	rows = nullptr;
}

inline b3SparseMat33::b3SparseMat33(u32 m)
{
	rowCount = m;
	rows = (b3RowValueList*)b3Alloc(rowCount * sizeof(b3RowValueList));
	for (u32 i = 0; i < rowCount; ++i)
	{
		new (rows + i)b3RowValueList();
	}
}

inline b3SparseMat33::b3SparseMat33(const b3SparseMat33& m)
{
	rowCount = m.rowCount;
	rows = (b3RowValueList*)b3Alloc(rowCount * sizeof(b3RowValueList));
	for (u32 i = 0; i < rowCount; ++i)
	{
		new (rows + i)b3RowValueList();
	}

	Copy(m);
}

inline b3SparseMat33::~b3SparseMat33()
{
	Destroy();
}

inline void b3SparseMat33::Destroy()
{
	for (u32 i = 0; i < rowCount; ++i)
	{
		b3RowValueList* vs = rows + i;

		b3RowValue* v = vs->head;
		while (v)
		{
			b3RowValue* v0 = v->next;
			b3Free(v);
			v = v0;
		}

		vs->~b3RowValueList();
	}

	b3Free(rows);
}

inline b3SparseMat33& b3SparseMat33::operator=(const b3SparseMat33& _m)
{
	if (_m.rows == rows)
	{
		return *this;
	}

	Destroy();

	rowCount = _m.rowCount;
	rows = (b3RowValueList*)b3Alloc(rowCount * sizeof(b3RowValueList));
	for (u32 i = 0; i < rowCount; ++i)
	{
		new (rows + i)b3RowValueList();
	}

	Copy(_m);

	return *this;
}

inline void b3SparseMat33::Copy(const b3SparseMat33& _m)
{
	B3_ASSERT(rowCount == _m.rowCount);

	for (u32 i = 0; i < rowCount; ++i)
	{
		b3RowValueList* vs1 = _m.rows + i;
		b3RowValueList* vs2 = rows + i;

		B3_ASSERT(vs2->count == 0);

		for (b3RowValue* v1 = vs1->head; v1; v1 = v1->next)
		{
			b3RowValue* v2 = (b3RowValue*)b3Alloc(sizeof(b3RowValue));

			v2->column = v1->column;
			v2->value = v1->value;

			vs2->PushFront(v2);
		}
	}
}

inline const b3Mat33& b3SparseMat33::operator()(u32 i, u32 j) const
{
	B3_ASSERT(i < rowCount);
	B3_ASSERT(j < rowCount);

	b3RowValueList* vs = rows + i;

	for (b3RowValue* v = vs->head; v; v = v->next)
	{
		if (v->column == j)
		{
			return v->value;
		}
	}

	return b3Mat33_zero;
}

inline b3Mat33& b3SparseMat33::operator()(u32 i, u32 j)
{
	B3_ASSERT(i < rowCount);
	B3_ASSERT(j < rowCount);

	b3RowValueList* vs = rows + i;

	for (b3RowValue* v = vs->head; v; v = v->next)
	{
		if (v->column == j)
		{
			return v->value;
		}
	}

	b3RowValue* v = (b3RowValue*)b3Alloc(sizeof(b3RowValue));
	v->column = j;
	v->value.SetZero();

	vs->PushFront(v);

	return v->value;
}

inline void b3SparseMat33::operator+=(const b3SparseMat33& m)
{
	B3_ASSERT(rowCount == m.rowCount);

	for (u32 i = 0; i < m.rowCount; ++i)
	{
		b3RowValueList* mvs = m.rows + i;

		for (b3RowValue* v = mvs->head; v; v = v->next)
		{
			u32 j = v->column;

			(*this)(i, j) += v->value;
		}
	}
}

inline void b3SparseMat33::operator-=(const b3SparseMat33& m)
{
	B3_ASSERT(rowCount == m.rowCount);

	for (u32 i = 0; i < m.rowCount; ++i)
	{
		b3RowValueList* mvs = m.rows + i;

		for (b3RowValue* v = mvs->head; v; v = v->next)
		{
			u32 j = v->column;

			(*this)(i, j) -= v->value;
		}
	}
}

inline void b3Add(b3SparseMat33& out, const b3SparseMat33& a, const b3SparseMat33& b)
{
	out = a;
	out += b;
}

inline void b3Sub(b3SparseMat33& out, const b3SparseMat33& a, const b3SparseMat33& b)
{
	out = a;
	out -= b;
}

inline void b3Mul(b3DenseVec3& out, const b3SparseMat33& A, const b3DenseVec3& v)
{
	B3_ASSERT(A.rowCount == out.n);

	out.SetZero();

	for (u32 i = 0; i < A.rowCount; ++i)
	{
		b3RowValueList* vs = A.rows + i;

		for (b3RowValue* vA = vs->head; vA; vA = vA->next)
		{
			u32 j = vA->column;
			b3Mat33 a = vA->value;

			out[i] += a * v[j];
		}
	}
}

inline void b3Mul(b3SparseMat33& out, float32 s, const b3SparseMat33& B)
{
	B3_ASSERT(out.rowCount == B.rowCount);

	if (s == 0.0f)
	{
		return;
	}

	out = B;

	for (u32 i = 0; i < out.rowCount; ++i)
	{
		b3RowValueList* vs = out.rows + i;
		for (b3RowValue* v = vs->head; v; v = v->next)
		{
			v->value = s * v->value;
		}
	}
}

inline b3SparseMat33 operator+(const b3SparseMat33& A, const b3SparseMat33& B)
{
	b3SparseMat33 result(A.rowCount);
	b3Add(result, A, B);
	return result;
}

inline b3SparseMat33 operator-(const b3SparseMat33& A, const b3SparseMat33& B)
{
	b3SparseMat33 result(A.rowCount);
	b3Sub(result, A, B);
	return result;
}

inline b3SparseMat33 operator*(float32 A, const b3SparseMat33& B)
{
	b3SparseMat33 result(B.rowCount);
	b3Mul(result, A, B);
	return result;
}

inline b3DenseVec3 operator*(const b3SparseMat33& A, const b3DenseVec3& v)
{
	b3DenseVec3 result(v.n);
	b3Mul(result, A, v);
	return result;
}

#endif
