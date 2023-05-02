#pragma once

#include "Mat44.h"
#include "Quat.h"

// A transform represents a rigid frame. It has a translation representing a position and a rotation quaternion representing an orientation relative to some reference frame.
class Transform
{
public:
	Transform() = default;

	// Set this transform from a translation vector and a rotation quaternion.
	Transform(const Vec3& _translation, const Quat& _rotation) : translation(_translation), rotation(_rotation) { }

	// Set this transform to the identity transform.
	void SetIdentity()
	{
		translation.SetZero();
		rotation.SetIdentity();
	}

	// Convert this transform to a 4-by-4 transformation matrix.
	Mat44 GetTransformMatrix() const
	{
		Vec3 t = translation;
		Mat33 R = rotation.GetRotationMatrix();

		return Mat44(
			Vec4(R.x.x, R.x.y, R.x.z, scalar(0)),
			Vec4(R.y.x, R.y.y, R.y.z, scalar(0)),
			Vec4(R.z.x, R.z.y, R.z.z, scalar(0)),
			Vec4(t.x,   t.y,   t.z,   scalar(1)));
	}

	Vec3 translation;
	Quat rotation;
};

// Identity transformation
extern const Transform Transform_identity;

// Multiply a transform times a vector.
inline Vec3 Mul(const Transform& T, const Vec3& v)
{
	return Mul(T.rotation, v) + T.translation;
}

// Multiply a transform times another transform.
inline Transform Mul(const Transform& A, const Transform& B)
{
	// [A y][B x] = [AB Ax+y]
	// [0 1][0 1]   [0  1   ]
	Transform C;
	C.rotation = Mul(A.rotation, B.rotation);
	C.translation = Mul(A.rotation, B.translation) + A.translation;
	return C;
}

// Multiply the transpose of one transform (inverse transform) times another transform (composed transform).
inline Transform MulT(const Transform& A, const Transform& B)
{
	//[A^-1  -A^-1*y][B x] = [A^-1*B A^-1(x-y)]
	//[0      1     ][0 1]   [0      1        ]
	Transform C;
	C.rotation = MulC(A.rotation, B.rotation);
	C.translation = MulC(A.rotation, B.translation - A.translation);
	return C;
}

// Multiply the transpose of a transform times a vector. If the transform represents a frame then this transforms the vector from one frame to another (inverse transform).
inline Vec3 MulT(const Transform& A, const Vec3& v)
{
	//[A^-1  -A^-1*y][x] = A^-1*x - A^-1*y = A^-1 * (x - y)
	//[0     1      ][1]   
	return MulC(A.rotation, v - A.translation);
}

// Inverse transform.
inline Transform Inverse(const Transform& T)
{
	Transform B;
	B.rotation = Conjugate(T.rotation);
	B.translation = MulC(T.rotation, -T.translation);
	return B;
}

// Multiply a transform times a vector. If the transform represents a frame this returns the vector in terms of the frame.
inline Vec3 operator*(const Transform& T, const Vec3& v)
{
	return Mul(T, v);
}

// Multiply a transform times another transform (composed transform).
inline Transform operator*(const Transform& A, const Transform& B)
{
	return Mul(A, B);
}