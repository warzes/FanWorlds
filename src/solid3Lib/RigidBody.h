#pragma once

#include "Dynamic.h"

class RigidBody : public Dynamic
{
public:
	RigidBody()
		: m_gravity(0.0f, 0.0f, 0.0f)
	{
	}

	virtual void proceed(MT_Scalar step);
	virtual void reset(const MT_Transform& xform);



	void backup();

	void applyForces(MT_Scalar step);

	void setGravity(const MT_Vector3& acceleration);
	void setDamping(MT_Scalar lin_damping, MT_Scalar ang_damping);

	void resolveCollision(const MT_Vector3& pos, MT_Scalar depth, const MT_Vector3& normal, MT_Scalar restitution, MT_Scalar friction);

private:
	MT_Transform     m_prev_xform;
	MT_Quaternion    m_prev_orn;
	MT_Vector3       m_gravity;
	MT_Scalar        m_lin_damping;
	MT_Scalar        m_ang_damping;
};

void resolveCollision(RigidBody& body1, const MT_Vector3& pos1,
	RigidBody& body2, const MT_Vector3& pos2,
	MT_Scalar depth, const MT_Vector3& normal, MT_Scalar restitution);