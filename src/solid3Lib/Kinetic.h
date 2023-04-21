#pragma once

#include "MT_Transform.h"
#include "MT_Point3.h"
#include "MT_Quaternion.h"

class Kinetic
{
public:
	Kinetic()
		: m_orn(0.0f, 0.0f, 0.0f, 1.0f),
		m_lin_vel(0.0f, 0.0f, 0.0f),
		m_ang_vel(0.0f, 0.0f, 0.0f)
	{
		m_xform.setIdentity();
	}

	virtual void proceed(MT_Scalar step);

	const MT_Point3&     getPosition() const { return m_xform.getOrigin(); }
	const MT_Quaternion& getOrientation() const { return m_orn; }

	const MT_Transform&  getTransform() const { return m_xform; }
	const MT_Vector3&    getLinearVelocity() const { return m_lin_vel; }
	const MT_Vector3&    getAngularVelocity() const { return m_ang_vel; }

	virtual void setTransform(const MT_Transform& xform);
	virtual void reset(const MT_Transform& xform);

	void setLinearVelocity(const MT_Vector3& lin_vel) { m_lin_vel = lin_vel; }
	void setAngularVelocity(const MT_Vector3& ang_vel) { m_ang_vel = ang_vel; }

	MT_Vector3 getVelocity(const MT_Vector3& rel_pos) const
	{
		return m_lin_vel + m_ang_vel.cross(rel_pos);
	}

	void translate(const MT_Vector3& v)
	{
		m_xform.getOrigin() += v;
	}

protected:
	MT_Transform  m_xform;
	MT_Quaternion m_orn;
	MT_Vector3    m_lin_vel;
	MT_Vector3    m_ang_vel;
};