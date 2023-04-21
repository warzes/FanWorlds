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

#ifndef B3_SOFT_BODY_NODE_H
#define B3_SOFT_BODY_NODE_H

#include <common/math/vec2.h>
#include <common/math/vec3.h>
#include <common/math/transform.h>

class b3SoftBody;

// Static node: Can be moved manually.
// Kinematic node: Non-zero velocity, can be moved by the solver.
// Dynamic node: Non-zero velocity determined by force, can be moved by the solver.
enum b3SoftBodyNodeType
{
	e_staticSoftBodyNode,
	e_kinematicSoftBodyNode,
	e_dynamicSoftBodyNode
};

// A soft body node.
struct b3SoftBodyNode 
{
public:
	// Set the node type.
	void SetType(b3SoftBodyNodeType type);

	// Get the node type.
	b3SoftBodyNodeType GetType() const;

	// Get the vertex index.
	u32 GetVertex() const;

	// Set the node position. 
	// If the node is dynamic changing the position directly might lead 
	// to physically incorrect simulation behaviour.
	void SetPosition(const b3Vec3& position);

	// Get the node position.
	const b3Vec3& GetPosition() const;

	// Set the node velocity.
	void SetVelocity(const b3Vec3& velocity);

	// Get the node velocity.
	const b3Vec3& GetVelocity() const;

	// Get the node mass.
	float32 GetMass() const;

	// Set the node mass damping.
	void SetMassDamping(float32 damping);

	// Get the node mass damping.
	float32 GetMassDamping() const;

	// Set the node radius.
	void SetRadius(float32 radius);
	
	// Get the node radius.
	float32 GetRadius() const;

	// Set the node coefficient of friction.
	void SetFriction(float32 friction);
	
	// Get the node coefficient of friction.
	float32 GetFriction() const;
	
	// Apply a force.
	void ApplyForce(const b3Vec3& force);
private:
	friend class b3SoftBody;
	friend class b3SoftBodyContactManager;
	friend class b3SoftBodySolver;
	friend class b3SoftBodyForceSolver;
	friend class b3SoftBodyContactSolver;
	friend class b3NodeBodyContact;

	b3SoftBodyNode() { }

	~b3SoftBodyNode() { }

	// Synchronize node
	void Synchronize(const b3Vec3& displacement);

	// Destroy associated contacts
	void DestroyContacts();

	// Type
	b3SoftBodyNodeType m_type;

	// Position
	b3Vec3 m_position;
	
	// Velocity
	b3Vec3 m_velocity;

	// Applied external force
	b3Vec3 m_force;

	// Mass
	float32 m_mass;

	// Inverse mass
	float32 m_invMass;

	// Mass damping
	float32 m_massDamping;

	// Radius
	float32 m_radius;

	// Coefficient of friction
	float32 m_friction;
	
	// User data. 
	void* m_userData;

	// Soft body mesh vertex index.
	u32 m_vertex;

	// Broadphase proxy
	u32 m_broadPhaseId;

	// Soft body
	b3SoftBody* m_body;
};

inline void b3SoftBodyNode::SetType(b3SoftBodyNodeType type)
{
	if (m_type == type)
	{
		return;
	}

	m_type = type;
	m_force.SetZero();

	if (type == e_staticSoftBodyNode)
	{
		m_velocity.SetZero();
		Synchronize(b3Vec3_zero);
	}

	DestroyContacts();
}

inline b3SoftBodyNodeType b3SoftBodyNode::GetType() const
{
	return m_type;
}

inline u32 b3SoftBodyNode::GetVertex() const
{
	return m_vertex;
}

inline void b3SoftBodyNode::SetPosition(const b3Vec3& position)
{
	b3Vec3 displacement = position - m_position;

	m_position = position;

	Synchronize(displacement);
}

inline const b3Vec3& b3SoftBodyNode::GetPosition() const
{
	return m_position;
}

inline void b3SoftBodyNode::SetVelocity(const b3Vec3& velocity)
{
	if (m_type == e_staticSoftBodyNode)
	{
		return;
	}
	m_velocity = velocity;
}

inline const b3Vec3& b3SoftBodyNode::GetVelocity() const
{
	return m_velocity;
}

inline float32 b3SoftBodyNode::GetMass() const
{
	return m_mass;
}

inline void b3SoftBodyNode::SetMassDamping(float32 massDamping)
{
	m_massDamping = massDamping;
}

inline float32 b3SoftBodyNode::GetMassDamping() const
{
	return m_massDamping;
}

inline void b3SoftBodyNode::SetRadius(float32 radius)
{
	m_radius = radius;
	
	Synchronize(b3Vec3_zero);
}

inline float32 b3SoftBodyNode::GetRadius() const
{
	return m_radius;
}

inline void b3SoftBodyNode::SetFriction(float32 friction)
{
	m_friction = friction;
}

inline float32 b3SoftBodyNode::GetFriction() const
{
	return m_friction;
}

inline void b3SoftBodyNode::ApplyForce(const b3Vec3& force)
{
	if (m_type != e_dynamicSoftBodyNode)
	{
		return;
	}
	m_force += force;
}

#endif