#include "stdafx.h"
#include "FollowCamera.h"

FollowCamera::FollowCamera(GameObject* pTarget, PxVehicleDrive4W* pVehicle, float pitch)
	: m_pTarget(pTarget)
	, m_pVehicle(pVehicle)
{
	SetPitch(pitch);
}

void FollowCamera::Initialize(const SceneContext& /*sceneContext*/)
{
	m_pCamera = new CameraComponent();
	AddComponent(m_pCamera);
}

void FollowCamera::Update(const SceneContext& /*sceneContext*/)
{
	if (m_pCamera->IsActive())
	{
		const auto targetTransform{ m_pTarget->GetTransform() };
		const auto targetPos{ XMLoadFloat3(&targetTransform->GetPosition()) };
		const auto speed{ std::clamp(std::powf(m_pVehicle->computeForwardSpeed() * 0.075f, 2.f), 0.f, m_MaxLookAhead) };

		const auto offset
		{ 
			XMVectorMultiply
			(
				XMLoadFloat3(&m_OffsetDir),
				XMVectorSet(m_OffsetDistance + speed, m_OffsetDistance + speed, m_OffsetDistance + speed, 0.f)
			)
		};

		const auto finalPos
		{
			XMVectorAdd
			(
				XMVectorAdd
				(
					targetPos,
					XMVectorMultiply
					(
						XMLoadFloat3(&targetTransform->GetForward()),
						XMVectorSet(speed, speed, speed, 0.f)
					)
				),
				offset
			)
		};
		GetTransform()->Translate(finalPos);
		
	}
}

void FollowCamera::SetPitch(float pitch, bool isDegrees)
{
	if (isDegrees)
		m_Pitch = XMConvertToRadians(pitch);
	else
		m_Pitch = pitch;

	CalculateOffsetDirection();
	const auto currPitch = GetTransform()->GetRotation().x;
	GetTransform()->Rotate(m_Pitch - currPitch, 0.f, 0.f, false);
}

void FollowCamera::CalculateOffsetDirection()
{
	XMStoreFloat3(&m_OffsetDir, XMVectorSet(0.f, std::sinf(m_Pitch), -std::cosf(m_Pitch), 0.f));
}