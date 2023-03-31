#include "stdafx.h"
#include "ACamera.h"

ACamera::ACamera(float fov, float near, float far)
    : m_fov(fov), m_near(near), m_far(far)
{
}

void ACamera::Update(float deltaTime)
{
    if( !m_targetActor ) return;

    if( m_targetActor->IsPendingDestroy() )
    {
        m_targetActor = nullptr;
    }

    GetTransform() = m_targetActor->GetTransform();
}

void ACamera::Draw(Renderer &renderer)
{
    renderer.SetCameraInfo(GetTransform().GetInverseMatrix(), m_fov, m_near, m_far);
}