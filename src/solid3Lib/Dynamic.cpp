#include "Dynamic.h"

void Dynamic::setMassProps(MT_Scalar mass, const MT_Vector3& inertia)
{
	m_inv_mass = mass != 0.0f ? 1.0f / mass : 0.0f;
	m_inv_inertia.setValue(inertia[0] != 0.0f ? 1.0f / inertia[0] : 0.0f,
		inertia[1] != 0.0f ? 1.0f / inertia[1] : 0.0f,
		inertia[2] != 0.0f ? 1.0f / inertia[2] : 0.0f);

}

void Dynamic::updateInertiaTensor()
{
	m_inv_inertia_tensor = m_xform.getBasis().scaled(m_inv_inertia) * m_xform.getBasis().transpose();
}

void Dynamic::proceed(MT_Scalar step)
{
	Kinetic::proceed(step);
	updateInertiaTensor();

	m_lin_vel += m_total_force * (m_inv_mass * step);
	m_ang_vel += m_inv_inertia_tensor * m_total_torque * step;
	clearForces();
}

void Dynamic::setTransform(const MT_Transform& xform)
{
	Kinetic::setTransform(xform);
	updateInertiaTensor();
}

void Dynamic::reset(const MT_Transform& xform)
{
	Kinetic::reset(xform);
	updateInertiaTensor();
	clearForces();
}