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

#include <cloth/forces/spring_force.h>
#include <cloth/particle.h>
#include <cloth/cloth_force_solver.h>
#include <sparse/dense_vec3.h>
#include <sparse/sparse_mat33.h>

void b3SpringForceDef::Initialize(b3Particle* particle1, b3Particle* particle2, float32 structuralStiffness, float32 dampingStiffness)
{
	type = e_springForce;
	p1 = particle1;
	p2 = particle2;
	b3Vec3 x1 = p1->GetPosition();
	b3Vec3 x2 = p2->GetPosition();
	restLength = b3Distance(x1, x2);
	structural = structuralStiffness;
	damping = dampingStiffness;
}

b3SpringForce::b3SpringForce(const b3SpringForceDef* def)
{
	m_type = e_springForce;
	m_p1 = def->p1;
	m_p2 = def->p2;
	m_L0 = def->restLength;
	m_ks = def->structural;
	m_kd = def->damping;
	m_f.SetZero();
}

b3SpringForce::~b3SpringForce()
{

}

void b3SpringForce::Apply(const b3ClothForceSolverData* data)
{
	b3DenseVec3& x = *data->x;
	b3DenseVec3& v = *data->v;
	b3DenseVec3& f = *data->f;
	b3SparseMat33& dfdx = *data->dfdx;
	b3SparseMat33& dfdv = *data->dfdv;

	u32 i1 = m_p1->m_solverId;
	u32 i2 = m_p2->m_solverId;

	b3Vec3 x1 = x[i1];
	b3Vec3 v1 = v[i1];

	b3Vec3 x2 = x[i2];
	b3Vec3 v2 = v[i2];

	b3Mat33 I; I.SetIdentity();

	m_f.SetZero();

	if (m_ks > 0.0f)
	{
		b3Vec3 dx = x1 - x2;

		float32 L = b3Length(dx);

		if (L > m_L0)
		{
			// Jacobian
			b3Vec3 dCdx = dx / L;

			m_f += -m_ks * (L - m_L0) * dCdx;

			// Force derivative
			b3Mat33 K11 = -m_ks * (b3Outer(dx, dx) + (1.0f - m_L0 / L) * (I - b3Outer(dx, dx)));
			b3Mat33 K12 = -K11;
			b3Mat33 K21 = K12;
			b3Mat33 K22 = K11;

			dfdx(i1, i1) += K11;
			dfdx(i1, i2) += K12;
			dfdx(i2, i1) += K21;
			dfdx(i2, i2) += K22;
		}
	}

	if (m_kd > 0.0f)
	{
		// C * J
		b3Vec3 dv = v1 - v2;

		m_f += -m_kd * dv;
		
		b3Mat33 K11 = -m_kd * I;
		b3Mat33 K12 = -K11;
		b3Mat33 K21 = K12;
		b3Mat33 K22 = K11;

		dfdv(i1, i1) += K11;
		dfdv(i1, i2) += K12;
		dfdv(i2, i1) += K21;
		dfdv(i2, i2) += K22;
	}

	f[i1] += m_f;
	f[i2] -= m_f;
}