#pragma once

#include "Precision.h"
#include "Core.h"
#include "Random.h"
#include "Particle.h"
#include "RigidBody.h"
#include "PContacts.h"
#include "PWorld.h"
#include "CollideFine.h"
#include "Contacts.h"
#include "fgen.h"
#include "Joints.h"

class GameApp final : public IApp
{
	bool Create() final;
	void Destroy() final;

	void Render() final;
	void Update(float deltaTime) final;

private:
	int m_windowWidth = 0;
	int m_windowHeight = 0;

	ShaderProgramRef m_shader;
	Uniform m_uniformProjectionMatrix;
	Uniform m_uniformViewMatrix;
	Uniform m_uniformWorldMatrix;

	glm::mat4 m_perspective;
	ModelRef m_model;
	Texture2DRef m_texture;
	Camera m_camera;

	void updateAdditionalMass();

	cyclone::ParticleWorld world = { 60 };
	cyclone::Particle *particleArray;
	cyclone::GroundContacts groundContactGenerator;

	cyclone::ParticleRod *rods;
	cyclone::Vector3 massPos = { 0,0,0.5f };
	cyclone::Vector3 massDisplayPos;

#define ROD_COUNT 15

#define BASE_MASS 1
#define EXTRA_MASS 10
};