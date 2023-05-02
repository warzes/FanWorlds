#include "stdafx.h"
#include "GJK.h"
#include "GJKProxy.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

// Implementation of the GJK (Gilbert-Johnson-Keerthi) algorithm 
// using Voronoi regions and Barycentric coordinates.

uint32_t b3_gjkCalls = 0, b3_gjkIters = 0, b3_gjkMaxIters = 0;

// Convert a point Q from Cartesian coordinates to Barycentric coordinates (u, v) 
// with respect to a segment AB.
// The last output value is the divisor.
static SE_FORCE_INLINE void Barycentric(scalar out[3],
	const Vec3& A, const Vec3& B,
	const Vec3& Q)
{
	Vec3 AB = B - A;
	Vec3 QA = A - Q;
	Vec3 QB = B - Q;

	//scalar divisor = Dot(AB, AB);

	out[0] = Dot(QB, AB);
	out[1] = -Dot(QA, AB);
	out[2] = out[0] + out[1];
}

// Convert a point Q from Cartesian coordinates to Barycentric coordinates (u, v, w) 
// with respect to a triangle ABC.
// The last output value is the divisor.
static SE_FORCE_INLINE void Barycentric(scalar out[4],
	const Vec3& A, const Vec3& B, const Vec3& C,
	const Vec3& Q)
{
	Vec3 AB = B - A;
	Vec3 AC = C - A;

	Vec3 QA = A - Q;
	Vec3 QB = B - Q;
	Vec3 QC = C - Q;

	Vec3 QB_x_QC = Cross(QB, QC);
	Vec3 QC_x_QA = Cross(QC, QA);
	Vec3 QA_x_QB = Cross(QA, QB);

	Vec3 AB_x_AC = Cross(AB, AC);

	//scalar divisor = Dot(AB_x_AC, AB_x_AC);

	out[0] = Dot(QB_x_QC, AB_x_AC);
	out[1] = Dot(QC_x_QA, AB_x_AC);
	out[2] = Dot(QA_x_QB, AB_x_AC);
	out[3] = out[0] + out[1] + out[2];
}

// Convert a point Q from Cartesian coordinates to Barycentric coordinates (u, v, w, x) 
// with respect to a tetrahedron ABCD.
// The last output value is the (positive) divisor.
static SE_FORCE_INLINE void Barycentric(scalar out[5],
	const Vec3& A, const Vec3& B, const Vec3& C, const Vec3& D,
	const Vec3& Q)
{
	Vec3 AB = B - A;
	Vec3 AC = C - A;
	Vec3 AD = D - A;

	Vec3 QA = A - Q;
	Vec3 QB = B - Q;
	Vec3 QC = C - Q;
	Vec3 QD = D - Q;

	scalar divisor = Det(AB, AC, AD);
	scalar sign = Sign(divisor);

	out[0] = sign * Det(QB, QC, QD);
	out[1] = sign * Det(QA, QD, QC);
	out[2] = sign * Det(QA, QB, QD);
	out[3] = sign * Det(QA, QC, QB);
	out[4] = sign * divisor;
}

Vec3 Simplex::GetSearchDirection(const Vec3& Q) const
{
	switch (m_count)
	{
	case 1:
	{
		return Q - m_vertices[0].point;
	}
	case 2:
	{
		Vec3 A = m_vertices[0].point;
		Vec3 B = m_vertices[1].point;

		Vec3 AB = B - A;
		Vec3 AQ = Q - A;
		Vec3 AB_x_AQ = Cross(AB, AQ);
		Vec3 PQ = Cross(AB_x_AQ, AB);
		return PQ;
	}
	case 3:
	{
		Vec3 A = m_vertices[0].point;
		Vec3 B = m_vertices[1].point;
		Vec3 C = m_vertices[2].point;

		Vec3 AB = B - A;
		Vec3 AC = C - A;
		Vec3 AQ = Q - A;
		Vec3 AB_x_AC = Cross(AB, AC);
		scalar sign = Dot(AB_x_AC, AQ);
		if (sign > scalar(0))
		{
			return AB_x_AC;
		}
		else
		{
			return -AB_x_AC;
		}
	}
	default:
	{
		assert(false);
		return Vec3(scalar(0), scalar(0), scalar(0));
	}
	}
}

Vec3 Simplex::GetClosestPoint() const
{
	switch (m_count)
	{
	case 0:
		assert(false);
		return Vec3(scalar(0), scalar(0), scalar(0));
	case 1:
		return m_vertices[0].point;
	case 2:
		return m_vertices[0].weight * m_vertices[0].point + m_vertices[1].weight * m_vertices[1].point;
	case 3:
		return m_vertices[0].weight * m_vertices[0].point + m_vertices[1].weight * m_vertices[1].point + m_vertices[2].weight * m_vertices[2].point;
	case 4:
		return Vec3(scalar(0), scalar(0), scalar(0));
	default:
		assert(false);
		return Vec3(scalar(0), scalar(0), scalar(0));
	}
}

void Simplex::GetClosestPoints(Vec3* p1, Vec3* p2) const
{
	switch (m_count)
	{
	case 0:
		assert(false);
		break;
	case 1:
		*p1 = m_vertices[0].point1;
		*p2 = m_vertices[0].point2;
		break;

	case 2:
		*p1 = m_vertices[0].weight * m_vertices[0].point1 + m_vertices[1].weight * m_vertices[1].point1;
		*p2 = m_vertices[0].weight * m_vertices[0].point2 + m_vertices[1].weight * m_vertices[1].point2;
		break;

	case 3:
		*p1 = m_vertices[0].weight * m_vertices[0].point1 + m_vertices[1].weight * m_vertices[1].point1 + m_vertices[2].weight * m_vertices[2].point1;
		*p2 = m_vertices[0].weight * m_vertices[0].point2 + m_vertices[1].weight * m_vertices[1].point2 + m_vertices[2].weight * m_vertices[2].point2;
		break;
	case 4:
		*p1 = m_vertices[0].weight * m_vertices[0].point1 + m_vertices[1].weight * m_vertices[1].point1 + m_vertices[2].weight * m_vertices[2].point1 + m_vertices[3].weight * m_vertices[3].point1;
		*p2 = *p1;
		break;
	default:
		assert(false);
		break;
	}
}

// Closest point on edge AB to Q.
void Simplex::Solve2(const Vec3& Q)
{
	SimplexVertex A = m_vertices[0];
	SimplexVertex B = m_vertices[1];

	// Test vertex regions
	scalar wAB[3];
	Barycentric(wAB, A.point, B.point, Q);

	// R A
	if (wAB[1] <= scalar(0))
	{
		m_count = 1;
		m_vertices[0] = A;
		m_vertices[0].weight = scalar(1);
		return;
	}

	// R B
	if (wAB[0] <= scalar(0))
	{
		m_count = 1;
		m_vertices[0] = B;
		m_vertices[0].weight = scalar(1);
		return;
	}

	// R AB
	scalar divisor = wAB[2];
	scalar s = scalar(1) / divisor;
	m_count = 2;
	m_vertices[0] = A;
	m_vertices[0].weight = s * wAB[0];
	m_vertices[1] = B;
	m_vertices[1].weight = s * wAB[1];
}

// Closest point on face ABC to Q.
// Voronoi regions: A, B, C, AB, AC, BC, ABC, ACB.
void Simplex::Solve3(const Vec3& Q)
{
	SimplexVertex A = m_vertices[0];
	SimplexVertex B = m_vertices[1];
	SimplexVertex C = m_vertices[2];

	// Test vertex regions
	scalar wAB[3], wBC[3], wCA[3];
	Barycentric(wAB, A.point, B.point, Q);
	Barycentric(wBC, B.point, C.point, Q);
	Barycentric(wCA, C.point, A.point, Q);

	// R A
	if (wAB[1] <= scalar(0) && wCA[0] <= scalar(0))
	{
		m_count = 1;
		m_vertices[0] = A;
		m_vertices[0].weight = scalar(1);
		return;
	}

	// R B
	if (wAB[0] <= scalar(0) && wBC[1] <= scalar(0))
	{
		m_count = 1;
		m_vertices[0] = B;
		m_vertices[0].weight = scalar(1);
		return;
	}

	// R C
	if (wBC[0] <= scalar(0) && wCA[1] <= scalar(0))
	{
		m_count = 1;
		m_vertices[0] = C;
		m_vertices[0].weight = scalar(1);
		return;
	}

	// Test edge regions		
	scalar wABC[4];
	Barycentric(wABC, A.point, B.point, C.point, Q);

	// This is used to help testing if the face degenerates 
	// into an edge.
	scalar area = wABC[3];

	// R AB
	if (wAB[0] > scalar(0) && wAB[1] > scalar(0) && area * wABC[2] <= scalar(0))
	{
		scalar divisor = wAB[2];
		assert(divisor > scalar(0));
		scalar s = scalar(1) / divisor;
		m_count = 2;
		m_vertices[0] = A;
		m_vertices[0].weight = s * wAB[0];
		m_vertices[1] = B;
		m_vertices[1].weight = s * wAB[1];
		return;
	}

	// R BC
	if (wBC[0] > scalar(0) && wBC[1] > scalar(0) && area * wABC[0] <= scalar(0))
	{
		scalar divisor = wBC[2];
		assert(divisor > scalar(0));
		scalar s = scalar(1) / divisor;
		m_count = 2;
		m_vertices[0] = B;
		m_vertices[0].weight = s * wBC[0];
		m_vertices[1] = C;
		m_vertices[1].weight = s * wBC[1];
		return;
	}

	// R CA
	if (wCA[0] > scalar(0) && wCA[1] > scalar(0) && area * wABC[1] <= scalar(0))
	{
		scalar divisor = wCA[2];
		assert(divisor > scalar(0));
		scalar s = scalar(1) / divisor;
		m_count = 2;
		m_vertices[0] = C;
		m_vertices[0].weight = s * wCA[0];
		m_vertices[1] = A;
		m_vertices[1].weight = s * wCA[1];
		return;
	}

	// R ABC/ACB
	scalar divisor = wABC[3];
	if (divisor <= scalar(0))
	{
		// Give up.
		return;
	}

	assert(divisor > scalar(0));
	scalar s = scalar(1) / divisor;
	m_count = 3;
	m_vertices[0] = A;
	m_vertices[0].weight = s * wABC[0];
	m_vertices[1] = B;
	m_vertices[1].weight = s * wABC[1];
	m_vertices[2] = C;
	m_vertices[2].weight = s * wABC[2];
}

// Closest point on tetrahedron A, B, C, D to Q.
void Simplex::Solve4(const Vec3& Q)
{
	SimplexVertex A = m_vertices[0];
	SimplexVertex B = m_vertices[1];
	SimplexVertex C = m_vertices[2];
	SimplexVertex D = m_vertices[3];

	// Test vertex regions
	scalar wAB[3], wAC[3], wAD[3], wBC[3], wCD[3], wDB[3];
	Barycentric(wAB, A.point, B.point, Q);
	Barycentric(wBC, B.point, C.point, Q);
	Barycentric(wAC, A.point, C.point, Q);
	Barycentric(wAD, A.point, D.point, Q);
	Barycentric(wCD, C.point, D.point, Q);
	Barycentric(wDB, D.point, B.point, Q);

	// R A
	if (wAB[1] <= scalar(0) && wAC[1] <= scalar(0) && wAD[1] <= scalar(0))
	{
		m_count = 1;
		m_vertices[0] = A;
		m_vertices[0].weight = scalar(1);
		return;
	}

	// R B
	if (wAB[0] <= scalar(0) && wDB[0] <= scalar(0) && wBC[1] <= scalar(0))
	{
		m_count = 1;
		m_vertices[0] = B;
		m_vertices[0].weight = scalar(1);
		return;
	}

	// R C
	if (wAC[0] <= scalar(0) && wBC[0] <= scalar(0) && wCD[1] <= scalar(0))
	{
		m_count = 1;
		m_vertices[0] = C;
		m_vertices[0].weight = scalar(1);
		return;
	}

	// R D
	if (wAD[0] <= scalar(0) && wCD[0] <= scalar(0) && wDB[1] <= scalar(0))
	{
		m_count = 1;
		m_vertices[0] = D;
		m_vertices[0].weight = scalar(1);
		return;
	}

	// Test edge regions
	scalar wACB[4], wABD[4], wADC[4], wBCD[4];
	Barycentric(wACB, A.point, C.point, B.point, Q);
	Barycentric(wABD, A.point, B.point, D.point, Q);
	Barycentric(wADC, A.point, D.point, C.point, Q);
	Barycentric(wBCD, B.point, C.point, D.point, Q);

	// R AB
	if (wABD[2] <= scalar(0) && wACB[1] <= scalar(0) && wAB[0] > scalar(0) && wAB[1] > scalar(0))
	{
		scalar divisor = wAB[2];
		assert(divisor > scalar(0));
		scalar s = scalar(1) / divisor;
		m_count = 2;
		m_vertices[0] = A;
		m_vertices[0].weight = s * wAB[0];
		m_vertices[1] = B;
		m_vertices[1].weight = s * wAB[1];
		return;
	}

	// R AC
	if (wACB[2] <= scalar(0) && wADC[1] <= scalar(0) && wAC[0] > scalar(0) && wAC[1] > scalar(0))
	{
		scalar divisor = wAC[2];
		assert(divisor > scalar(0));
		scalar s = scalar(1) / divisor;
		m_count = 2;
		m_vertices[0] = A;
		m_vertices[0].weight = s * wAC[0];
		m_vertices[1] = C;
		m_vertices[1].weight = s * wAC[1];
		return;
	}

	// R AD
	if (wADC[2] <= scalar(0) && wABD[1] <= scalar(0) && wAD[0] > scalar(0) && wAD[1] > scalar(0))
	{
		scalar divisor = wAD[2];
		assert(divisor > scalar(0));
		scalar s = scalar(1) / divisor;
		m_count = 2;
		m_vertices[0] = A;
		m_vertices[0].weight = s * wAD[0];
		m_vertices[1] = D;
		m_vertices[1].weight = s * wAD[1];
		return;
	}

	// R BC
	if (wACB[0] <= scalar(0) && wBCD[2] <= scalar(0) && wBC[0] > scalar(0) && wBC[1] > scalar(0))
	{
		scalar divisor = wBC[2];
		assert(divisor > scalar(0));
		scalar s = scalar(1) / divisor;
		m_count = 2;
		m_vertices[0] = B;
		m_vertices[0].weight = s * wBC[0];
		m_vertices[1] = C;
		m_vertices[1].weight = s * wBC[1];
		return;
	}

	// R CD
	if (wADC[0] <= scalar(0) && wBCD[0] <= scalar(0) && wCD[0] > scalar(0) && wCD[1] > scalar(0))
	{
		scalar divisor = wCD[2];
		assert(divisor > scalar(0));
		scalar s = scalar(1) / divisor;
		m_count = 2;
		m_vertices[0] = C;
		m_vertices[0].weight = s * wCD[0];
		m_vertices[1] = D;
		m_vertices[1].weight = s * wCD[1];
		return;
	}

	// R DB
	if (wABD[0] <= scalar(0) && wBCD[1] <= scalar(0) && wDB[0] > scalar(0) && wDB[1] > scalar(0))
	{
		scalar divisor = wDB[2];
		assert(divisor > scalar(0));
		scalar s = scalar(1) / divisor;
		m_count = 2;
		m_vertices[0] = D;
		m_vertices[0].weight = s * wDB[0];
		m_vertices[1] = B;
		m_vertices[1].weight = s * wDB[1];
		return;
	}

	// Test face regions
	scalar wABCD[5];
	Barycentric(wABCD, A.point, B.point, C.point, D.point, Q);

	// R ACB 
	if (wABCD[3] <= scalar(0) && wACB[0] > scalar(0) && wACB[1] > scalar(0) && wACB[2] > scalar(0))
	{
		scalar divisor = wACB[0] + wACB[1] + wACB[2];
		assert(divisor > scalar(0));
		scalar s = scalar(1) / divisor;
		m_count = 3;
		m_vertices[0] = A;
		m_vertices[0].weight = s * wACB[0];
		m_vertices[1] = C;
		m_vertices[1].weight = s * wACB[1];
		m_vertices[2] = B;
		m_vertices[2].weight = s * wACB[2];
		return;
	}

	// R ABD
	if (wABCD[2] <= scalar(0) && wABD[0] > scalar(0) && wABD[1] > scalar(0) && wABD[2] > scalar(0))
	{
		scalar divisor = wABD[0] + wABD[1] + wABD[2];
		assert(divisor > scalar(0));
		scalar s = scalar(1) / divisor;
		m_count = 3;
		m_vertices[0] = A;
		m_vertices[0].weight = s * wABD[0];
		m_vertices[1] = B;
		m_vertices[1].weight = s * wABD[1];
		m_vertices[2] = D;
		m_vertices[2].weight = s * wABD[2];
		return;
	}

	// R ADC
	if (wABCD[1] <= scalar(0) && wADC[0] > scalar(0) && wADC[1] > scalar(0) && wADC[2] > scalar(0))
	{
		scalar divisor = wADC[0] + wADC[1] + wADC[2];
		assert(divisor > scalar(0));
		scalar s = scalar(1) / divisor;
		m_count = 3;
		m_vertices[0] = A;
		m_vertices[0].weight = s * wADC[0];
		m_vertices[1] = D;
		m_vertices[1].weight = s * wADC[1];
		m_vertices[2] = C;
		m_vertices[2].weight = s * wADC[2];
		return;
	}

	// R BCD
	if (wABCD[0] <= scalar(0) && wBCD[0] > scalar(0) && wBCD[1] > scalar(0) && wBCD[2] > scalar(0))
	{
		scalar divisor = wBCD[0] + wBCD[1] + wBCD[2];
		assert(divisor > scalar(0));
		scalar s = scalar(1) / divisor;
		m_count = 3;
		m_vertices[0] = B;
		m_vertices[0].weight = s * wBCD[0];
		m_vertices[1] = C;
		m_vertices[1].weight = s * wBCD[1];
		m_vertices[2] = D;
		m_vertices[2].weight = s * wBCD[2];
		return;
	}

	// R ABCD
	scalar divisor = wABCD[0] + wABCD[1] + wABCD[2] + wABCD[3];
	if (divisor <= scalar(0))
	{
		// Give up.
		return;
	}

	assert(divisor > scalar(0));
	scalar s = scalar(1) / divisor;
	m_count = 4;
	m_vertices[0].weight = s * wABCD[0];
	m_vertices[1].weight = s * wABCD[1];
	m_vertices[2].weight = s * wABCD[2];
	m_vertices[3].weight = s * wABCD[3];
}

///////////////////////////////////////////////////////////////////////////////////////////////////

GJKOutput GJK(const Transform& xf1, const GJKProxy& proxy1,
	const Transform& xf2, const GJKProxy& proxy2,
	bool applyRadius, SimplexCache* cache)
{
	++b3_gjkCalls;

	// Initialize the simplex.
	Simplex simplex;
	simplex.ReadCache(cache, xf1, proxy1, xf2, proxy2);

	// Get simplex vertices as an array.
	SimplexVertex* vertices = simplex.m_vertices;

	// These store the vertices of the last simplex so that we
	// can check for duplicates and prevent cycling.
	uint32_t save1[4], save2[4];
	uint32_t saveCount = 0;

	// Last iteration squared distance for checking if we're getting close
	// to the origin and prevent cycling.
	scalar distSq1 = SE_MAX_SCALAR;

	const Vec3 kOrigin(scalar(0), scalar(0), scalar(0));

	// Limit number of iterations to prevent cycling.
	const uint32_t kMaxIters = 20;

	// Main iteration loop.
	uint32_t iter = 0;
	while (iter < kMaxIters)
	{
		// Copy simplex so we can identify duplicates.
		saveCount = simplex.m_count;
		for (uint32_t i = 0; i < saveCount; ++i)
		{
			save1[i] = vertices[i].index1;
			save2[i] = vertices[i].index2;
		}

		// Determine the closest point on the simplex and
		// remove unused vertices.
		switch (simplex.m_count)
		{
		case 1:
			break;
		case 2:
			simplex.Solve2(kOrigin);
			break;
		case 3:
			simplex.Solve3(kOrigin);
			break;
		case 4:
			simplex.Solve4(kOrigin);
			break;
		default:
			assert(false);
			break;
		}

		// If we have 4 points, then the origin is in the corresponding tethrahedron.
		if (simplex.m_count == 4)
		{
			break;
		}

		// Compute the closest point.
		Vec3 p = simplex.GetClosestPoint();
		scalar distSq2 = Dot(p, p);
		// Ensure we're getting close to the origin.
		if (distSq2 >= distSq1)
		{
			//break;
		}
		distSq1 = distSq2;

		// Get search direction.
		Vec3 d = simplex.GetSearchDirection(kOrigin);

		// Ensure the search direction is non-zero.
		if (Dot(d, d) < SE_EPSILON * SE_EPSILON)
		{
			break;
		}

		// Compute a tentative new simplex vertex using support points.
		SimplexVertex* vertex = vertices + simplex.m_count;
		vertex->index1 = proxy1.GetSupportIndex(MulC(xf1.rotation, -d));
		vertex->point1 = Mul(xf1, proxy1.GetVertex(vertex->index1));
		vertex->index2 = proxy2.GetSupportIndex(MulC(xf2.rotation, d));
		vertex->point2 = Mul(xf2, proxy2.GetVertex(vertex->index2));
		vertex->point = vertex->point2 - vertex->point1;

		// Iteration count is equated to the number of support point calls.
		++iter;
		++b3_gjkIters;

		// Check for duplicate support points. 
		// This is the main termination criteria.
		bool duplicate = false;
		for (uint32_t i = 0; i < saveCount; ++i)
		{
			if (vertex->index1 == save1[i] && vertex->index2 == save2[i])
			{
				duplicate = true;
				break;
			}
		}

		// If we found a duplicate support point we must exit to avoid cycling.
		if (duplicate)
		{
			break;
		}

		// New vertex is ok and needed.
		++simplex.m_count;
	}

	b3_gjkMaxIters = glm::max(b3_gjkMaxIters, iter);

	// Prepare result.
	GJKOutput output;
	simplex.GetClosestPoints(&output.point1, &output.point2);
	output.distance = Distance(output.point1, output.point2);
	output.iterations = iter;

	// Cache the simplex.
	simplex.WriteCache(cache);

	// Apply radius if requested.
	if (applyRadius)
	{
		scalar r1 = proxy1.radius;
		scalar r2 = proxy2.radius;

		if (output.distance > r1 + r2 && output.distance > SE_EPSILON)
		{
			// Shapes are still no overlapped.
			// Move the witness points to the outer surface.
			output.distance -= r1 + r2;
			Vec3 d = output.point2 - output.point1;
			Vec3 normal = Normalize(d);
			output.point1 += r1 * normal;
			output.point2 -= r2 * normal;
		}
		else
		{
			// Shapes are overlapped when radii are considered.
			// Move the witness points to the middle.
			Vec3 p = scalar(0.5) * (output.point1 + output.point2);
			output.point1 = p;
			output.point2 = p;
			output.distance = scalar(0);
		}
	}

	// Output result.
	return output;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t b3_gjkCacheHits = 0;

// Implements Simplex routines for a cached simplex.
void Simplex::ReadCache(const SimplexCache* cache,
	const Transform& xf1, const GJKProxy& proxy1,
	const Transform& xf2, const GJKProxy& proxy2)
{
	assert(cache->count <= 4);
	m_count = (uint8_t)cache->count;
	for (uint32_t i = 0; i < m_count; ++i)
	{
		SimplexVertex* v = m_vertices + i;
		v->index1 = cache->index1[i];
		v->index2 = cache->index2[i];
		Vec3 wALocal = proxy1.GetVertex(v->index1);
		Vec3 wBLocal = proxy2.GetVertex(v->index2);
		v->point1 = xf1 * wALocal;
		v->point2 = xf2 * wBLocal;
		v->point = v->point2 - v->point1;
		v->weight = scalar(0);
	}

	// Compute the new simplex metric
	// If it is substantially different than
	// old metric then flush the simplex.
	if (m_count > 1)
	{
		scalar metric1 = cache->metric;
		scalar metric2 = GetMetric();
		if (metric2 < scalar(0.5) * metric1 || scalar(2) * metric1 < metric2 || metric2 < SE_EPSILON)
		{
			// Flush
			m_count = 0;
		}
		else
		{
			++b3_gjkCacheHits;
		}
	}

	// If cache is empty or flushed choose an arbitrary simplex.
	if (m_count == 0)
	{
		SimplexVertex* v = m_vertices + 0;
		Vec3 w1Local = proxy1.GetVertex(0);
		Vec3 w2Local = proxy2.GetVertex(0);
		v->point1 = Mul(xf1, w1Local);
		v->point2 = Mul(xf2, w2Local);
		v->point = v->point2 - v->point1;
		v->weight = scalar(1);
		v->index1 = 0;
		v->index2 = 0;
		m_count = 1;
	}
}

void Simplex::WriteCache(SimplexCache* cache) const
{
	cache->metric = GetMetric();
	cache->count = uint16_t(m_count);
	for (uint32_t i = 0; i < m_count; ++i)
	{
		cache->index1[i] = uint8_t(m_vertices[i].index1);
		cache->index2[i] = uint8_t(m_vertices[i].index2);
	}
}

scalar Simplex::GetMetric() const
{
	switch (m_count)
	{
	case 0:
		assert(false);
		return scalar(0);
	case 1:
		return scalar(0);
	case 2:
		// Magnitude
		return Distance(m_vertices[0].point, m_vertices[1].point);
	case 3:
	{
		// Area
		Vec3 E1 = m_vertices[1].point - m_vertices[0].point;
		Vec3 E2 = m_vertices[2].point - m_vertices[0].point;
		return Length(Cross(E1, E2));
	}
	case 4:
	{
		// Volume
		Vec3 E1 = m_vertices[1].point - m_vertices[0].point;
		Vec3 E2 = m_vertices[2].point - m_vertices[0].point;
		Vec3 E3 = m_vertices[3].point - m_vertices[0].point;
		scalar det = Det(E1, E2, E3);
		scalar sign = Sign(det);
		scalar volume = sign * det;
		return volume;
	}
	default:
		assert(false);
		return scalar(0);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

GJKOutput GJK(const Transform& xf1, const GJKProxy& proxy1,
	const Transform& xf2, const GJKProxy& proxy2, bool applyRadius)
{
	SimplexCache cache;
	cache.count = 0;
	return GJK(xf1, proxy1, xf2, proxy2, applyRadius, &cache);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

// The output of the GJK-based shape cast algorithm.
struct ShapeCastOutput
{
	scalar t; // time of impact
	Vec3 point; // contact point at t
	Vec3 normal; // contact normal at t
	uint32_t iterations; // number of iterations 
};

// Gino van der Bergen
// "Smooth Mesh Contacts with GJK"
// Game Physics Pearls 2010, page 99
bool ShapeCast(ShapeCastOutput* output,
	const Transform& xf1, const GJKProxy& proxy1,
	const Transform& xf2, const GJKProxy& proxy2, const Vec3& translation2)
{
	scalar r1 = proxy1.radius;
	scalar r2 = proxy2.radius;
	scalar radius = r1 + r2;

	Vec3 r = translation2;

	scalar t = scalar(0);
	Vec3 n = Vec3_zero;

	uint32_t index1 = proxy1.GetSupportIndex(MulC(xf1.rotation, -r));
	uint32_t index2 = proxy2.GetSupportIndex(MulC(xf2.rotation, r));
	Vec3 w1 = xf1 * proxy1.GetVertex(index1);
	Vec3 w2 = xf2 * proxy2.GetVertex(index2);
	Vec3 v = w1 - w2;

	Simplex simplex;
	simplex.m_count = 0;

	SimplexVertex* vertices = simplex.m_vertices;

	uint32_t save1[4], save2[4];
	uint32_t saveCount = 0;

	const uint32_t kMaxIters = 20;
	const scalar kTolerance = scalar(10) * SE_EPSILON;

	scalar maxTolerance = scalar(1);

	uint32_t iter = 0;
	while (iter < kMaxIters && glm::abs(LengthSquared(v) - radius * radius) > kTolerance * maxTolerance)
	{
		// Support in direction -v
		index1 = proxy1.GetSupportIndex(MulC(xf1.rotation, -v));
		index2 = proxy2.GetSupportIndex(MulC(xf2.rotation, v));
		w1 = xf1 * proxy1.GetVertex(index1);
		w2 = xf2 * proxy2.GetVertex(index2);
		Vec3 p = w1 - w2;

		// Support plane on boundary of CSO is (-v, p)
		// -v is normal at p
		scalar vp = Dot(v, p);
		scalar vr = Dot(v, r);

		if (vp - radius > t * vr)
		{
			if (vr > scalar(0))
			{
				t = (vp - radius) / vr;

				if (t > scalar(1))
				{
					output->iterations = iter;
					return false;
				}

				n = -v;

				// Flush the simplex
				simplex.m_count = 0;
				saveCount = 0;
			}
			else
			{
				output->iterations = iter;
				return false;
			}
		}

		// Unite p - s to simplex
		Vec3 s = t * r;

		SimplexVertex* vertex = vertices + simplex.m_count;
		vertex->index1 = index1;
		vertex->point1 = w1;
		vertex->index2 = index2;
		vertex->point2 = w2;
		vertex->point = p - s;

		// If we found a duplicate support point we must exit to avoid cycling.
		bool duplicate = false;
		for (uint32_t i = 0; i < saveCount; ++i)
		{
			if (vertex->index1 == save1[i] && vertex->index2 == save2[i])
			{
				duplicate = true;
				break;
			}
		}

		if (duplicate)
		{
			break;
		}

		++simplex.m_count;

		// Compute tolerance
		maxTolerance = -SE_EPSILON;
		for (uint32_t i = 0; i < simplex.m_count; ++i)
		{
			maxTolerance = glm::max(maxTolerance, LengthSquared(vertices[i].point));
		}

		// Copy simplex so we can identify duplicates.
		saveCount = simplex.m_count;
		for (uint32_t i = 0; i < saveCount; ++i)
		{
			save1[i] = vertices[i].index1;
			save2[i] = vertices[i].index2;
		}

		// Sub-solve
		const Vec3 origin = Vec3_zero;

		switch (simplex.m_count)
		{
		case 1:
			break;
		case 2:
			simplex.Solve2(origin);
			break;
		case 3:
			simplex.Solve3(origin);
			break;
		case 4:
			simplex.Solve4(origin);
			break;
		default:
			assert(false);
			break;
		}

		if (simplex.m_count == 4)
		{
			break;
		}

		v = simplex.GetClosestPoint();

		++iter;
	}

	// Prepare output.
	Vec3 point1, point2;
	simplex.GetClosestPoints(&point1, &point2);

	if (LengthSquared(v) > SE_EPSILON * SE_EPSILON)
	{
		n = -v;
	}

	n.Normalize();

	output->t = t;
	output->point = point1 + r1 * n;
	output->normal = n;
	output->iterations = iter;
	return true;
}