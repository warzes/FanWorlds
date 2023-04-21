#include "Kinetic.h"

void Kinetic::proceed(MT_Scalar step)
{
	m_xform.setOrigin(m_xform.getOrigin() + m_lin_vel * step);
	m_orn += m_ang_vel * m_orn * (step * 0.5f);
	m_orn.normalize();
	m_xform.setRotation(m_orn);
}

void Kinetic::setTransform(const MT_Transform& xform)
{
	m_xform = xform;
	m_xform.getBasis().getRotation(m_orn);
}

void Kinetic::reset(const MT_Transform& xform)
{
	setTransform(xform);
	m_lin_vel.setValue(0.0f, 0.0f, 0.0f);
	m_ang_vel.setValue(0.0f, 0.0f, 0.0f);
}