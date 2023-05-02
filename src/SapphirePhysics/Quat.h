#pragma once

#include "MicroMath.h"
#include "Mat33.h"

class Quat
{
public:
	Quat() = default;

	// Set this quaternion from four values.
	Quat(scalar _x, scalar _y, scalar _z, scalar _s) : v(_x, _y, _z), s(_s) { }

	// Add a quaternion to this quaternion.
	void operator+=(const Quat& q)
	{
		v.x += q.v.x;
		v.y += q.v.y;
		v.z += q.v.z;
		s += q.s;
	}

	// Subtract a quaternion from this quaternion.
	void operator-=(const Quat& q)
	{
		v.x -= q.v.x;
		v.y -= q.v.y;
		v.z -= q.v.z;
		s -= q.s;
	}

	// Set this quaternion to identity.
	void SetIdentity()
	{
		v.x = v.y = v.z = scalar(0);
		s = scalar(1);
	}

	// Set this quaternion from four values.
	void Set(scalar _x, scalar _y, scalar _z, scalar _s)
	{
		v.x = _x;
		v.y = _y;
		v.z = _z;
		s = _s;
	}

	// Convert this quaternion to the unit quaternion. Return the length.
	scalar Normalize()
	{
		scalar len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + s * s);
		if (len > SE_EPSILON)
		{
			scalar inv_len = scalar(1) / len;
			v *= inv_len;
			s *= inv_len;
		}
		return len;
	}

	// Set this quaternion from an axis and full angle of rotation about the axis.
	void SetAxisAngle(const Vec3& axis, scalar angle)
	{
		scalar theta = scalar(0.5) * angle;

		v = sin(theta) * axis;
		s = cos(theta);
	}

	// If this quaternion represents an orientation output the axis and angle of rotation about the axis.
	void GetAxisAngle(Vec3* axis, scalar* angle) const
	{
		// sin^2 = 1 - cos^2
		// sin = sqrt( sin^2 ) = ||v||
		// axis = v / sin
		scalar sine = Length(v);
		axis->SetZero();
		if (sine > SE_EPSILON)
		{
			scalar inv_sine = scalar(1) / sine;
			*axis = inv_sine * v;
		}

		// cosine check
		scalar cosine = glm::clamp(s, scalar(-1), scalar(1));

		// half angle
		scalar theta = acos(cosine);

		// full angle
		*angle = scalar(2) * theta;
	}

	// Get the x axis.
	Vec3 GetXAxis() const
	{
		scalar x = v.x, y = v.y, z = v.z, w = s;

		scalar y2 = y + y, z2 = z + z;
		scalar xy = x * y2, xz = x * z2;
		scalar yy = y * y2, zz = z * z2;
		scalar wy = w * y2, wz = w * z2;

		return Vec3(scalar(1) - (yy + zz), xy + wz, xz - wy);
	}

	// Get the y axis.
	Vec3 GetYAxis() const
	{
		scalar x = v.x, y = v.y, z = v.z, w = s;

		scalar x2 = x + x, y2 = y + y, z2 = z + z;
		scalar xx = x * x2, xy = x * y2;
		scalar yz = y * z2, zz = z * z2;
		scalar wx = w * x2, wz = w * z2;

		return Vec3(xy - wz, scalar(1) - (xx + zz), yz + wx);
	}

	// Get the z axis.
	Vec3 GetZAxis() const
	{
		scalar x = v.x, y = v.y, z = v.z, w = s;

		scalar x2 = x + x, y2 = y + y, z2 = z + z;
		scalar xx = x * x2, xz = x * z2;
		scalar yy = y * y2, yz = y * z2;
		scalar wx = w * x2, wy = w * y2;

		return Vec3(xz + wy, yz - wx, scalar(1) - (xx + yy));
	}

	// Get the x, y, z axes.
	Mat33 GetRotationMatrix() const
	{
		scalar x = v.x, y = v.y, z = v.z, w = s;

		scalar x2 = x + x, y2 = y + y, z2 = z + z;
		scalar xx = x * x2, xy = x * y2, xz = x * z2;
		scalar yy = y * y2, yz = y * z2, zz = z * z2;
		scalar wx = w * x2, wy = w * y2, wz = w * z2;

		return Mat33(
			Vec3(scalar(1) - (yy + zz), xy + wz, xz - wy),
			Vec3(xy - wz, scalar(1) - (xx + zz), yz + wx),
			Vec3(xz + wy, yz - wx, scalar(1) - (xx + yy)));
	}

	// Get the angle about the x axis.
	scalar GetXAngle() const
	{
		return atan2(v.x, s);
	}

	// Get the angle about the y axis.
	scalar GetYAngle() const
	{
		return atan2(v.y, s);
	}

	// Get the angle about the z axis.
	scalar GetZAngle() const
	{
		return atan2(v.z, s);
	}

	Vec3 v;
	scalar s;
};

// Identity quaternion
extern const Quat Quat_identity;

// Add two quaternions.
inline Quat operator+(const Quat& a, const Quat& b)
{
	return Quat(a.v.x + b.v.x, a.v.y + b.v.y, a.v.z + b.v.z, a.s + b.s);
}

// Subtract two quaternions.
inline Quat operator-(const Quat& a, const Quat& b)
{
	return Quat(a.v.x - b.v.x, a.v.y - b.v.y, a.v.z - b.v.z, a.s - b.s);
}

// Multiply a quaternion by a scalar.
inline Quat operator*(scalar s, const Quat& q)
{
	return Quat(s * q.v.x, s * q.v.y, s * q.v.z, s * q.s);
}

// Negate a quaternion.
inline Quat operator-(const Quat& q)
{
	return Quat(-q.v.x, -q.v.y, -q.v.z, -q.s);
}

// Multiply two quaternions.
inline Quat Mul(const Quat& a, const Quat& b)
{
	Quat result;
	result.v = Cross(a.v, b.v) + a.s * b.v + b.s * a.v;
	result.s = a.s * b.s - Dot(a.v, b.v);
	return result;
}

// Multiply two quaternions.
inline Quat operator*(const Quat& a, const Quat& b)
{
	return Mul(a, b);
}

// Perform the dot poduct of two quaternions.
inline scalar Dot(const Quat& a, const Quat& b)
{
	return a.v.x * b.v.x + a.v.y * b.v.y + a.v.z * b.v.z + a.s * b.s;
}

// Return the conjugate of a quaternion.
// If the quaternion is unit this returns its inverse.
inline Quat Conjugate(const Quat& q)
{
	return Quat(-q.v.x, -q.v.y, -q.v.z, q.s);
}

// Multiply the conjugate of a quaternion times another quaternion.
inline Quat MulC(const Quat& a, const Quat& b)
{
	return Mul(Conjugate(a), b);
}

// Return the length of a quaternion.
inline scalar Length(const Quat& q)
{
	return std::sqrt(q.v.x * q.v.x + q.v.y * q.v.y + q.v.z * q.v.z + q.s * q.s);
}

// Convert a quaternion to the unit quaternion.
inline Quat Normalize(const Quat& q)
{
	scalar s = Length(q);
	if (s > SE_EPSILON)
	{
		s = scalar(1) / s;
		return s * q;
	}
	return Quat(scalar(0), scalar(0), scalar(0), scalar(1));
}

// Rotate a vector.
inline Vec3 Mul(const Quat& q, const Vec3& v)
{
	Vec3 t = scalar(2) * Cross(q.v, v);
	return v + q.s * t + Cross(q.v, t);
}

// Inverse rotate a vector.
inline Vec3 MulC(const Quat& q, const Vec3& v)
{
	return Mul(Conjugate(q), v);
}

// Convert a 3-by3 rotation matrix to a rotation quaternion.
inline Quat Mat33Quat(const Mat33& m)
{
	// Check the diagonal.
	scalar trace = m[0][0] + m[1][1] + m[2][2];

	if (trace > scalar(0))
	{
		Quat result;

		scalar s = std::sqrt(trace + scalar(1));
		result.s = scalar(0.5) * s;

		scalar t = scalar(0.5) / s;
		result.v.x = t * (m[1][2] - m[2][1]);
		result.v.y = t * (m[2][0] - m[0][2]);
		result.v.z = t * (m[0][1] - m[1][0]);

		return result;
	}

	// Diagonal is negative.
	const uint32_t next[3] = { 1, 2, 0 };

	uint32_t i = 0;

	if (m[1][1] > m[0][0])
	{
		i = 1;
	}

	if (m[2][2] > m[i][i])
	{
		i = 2;
	}

	uint32_t j = next[i];
	uint32_t k = next[j];

	scalar s = std::sqrt((m[i][i] - (m[j][j] + m[k][k])) + scalar(1));

	scalar q[4];
	q[i] = s * scalar(0.5);

	scalar t;
	if (s != scalar(0))
	{
		t = scalar(0.5) / s;
	}
	else
	{
		t = s;
	}

	q[3] = t * (m[j][k] - m[k][j]);
	q[j] = t * (m[i][j] + m[j][i]);
	q[k] = t * (m[i][k] + m[k][i]);

	Quat result;
	result.v.x = q[0];
	result.v.y = q[1];
	result.v.z = q[2];
	result.s = q[3];
	return result;
}

// Rotation about the x-axis.
inline Quat QuatRotationX(scalar angle)
{
	scalar x = scalar(0.5) * angle;

	Quat q;
	q.v.x = sin(x);
	q.v.y = scalar(0);
	q.v.z = scalar(0);
	q.s = cos(x);
	return q;
}

// Rotation about the y-axis.
inline Quat QuatRotationY(scalar angle)
{
	scalar x = scalar(0.5) * angle;

	Quat q;
	q.v.x = scalar(0);
	q.v.y = sin(x);
	q.v.z = scalar(0);
	q.s = cos(x);
	return q;
}

// Rotation about the z-axis.
inline Quat QuatRotationZ(scalar angle)
{
	scalar x = scalar(0.5) * angle;

	Quat q;
	q.v.x = scalar(0);
	q.v.y = scalar(0);
	q.v.z = sin(x);
	q.s = cos(x);
	return q;
}

// Rotation between two normal vectors.
inline Quat QuatRotationBetween(const Vec3& a, const Vec3& b)
{
	// |a x b| = sin(theta)
	// a . b = cos(theta)
	// sin(theta / 2) = +/- sqrt([1 - cos(theta)] / 2)
	// cos(theta / 2) = +/- sqrt([1 + cos(theta)] / 2)
	// q.v = sin(theta / 2) * (a x b) / |a x b|
	// q.s = cos(theta / 2)
	Quat q;

	Vec3 axis = Cross(a, b);
	scalar s = Length(axis);
	scalar c = Dot(a, b);
	if (s > SE_EPSILON)
	{
		q.v = std::sqrt(scalar(0.5) * (scalar(1) - c)) * (axis / s);
		q.s = std::sqrt(scalar(0.5) * (scalar(1) + c));
	}
	else
	{
		q.SetIdentity();
	}

	return q;
}