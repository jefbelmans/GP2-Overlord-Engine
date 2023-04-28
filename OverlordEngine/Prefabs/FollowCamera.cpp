#include "stdafx.h"
#include "FollowCamera.h"

FollowCamera::FollowCamera(GameObject* pTarget, PxVehicleTelemetryData* pTelemetryData, const XMFLOAT3& offset)
	: m_pTarget(pTarget)
	, m_pTelemetryData(pTelemetryData)
	, m_Offset(offset)
{}

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
		const auto offset{ XMLoadFloat3(&m_Offset) };
		const auto forward{ XMLoadFloat3(&targetTransform->GetForward()) };
		const auto velocityMag{ m_pTarget->GetComponent<RigidBodyComponent>()->GetPxRigidActor()
			->is<PxRigidDynamic>()->getLinearVelocity().magnitude() };

		// Calculate position
		const auto finalPos =
			XMVectorAdd
			(
				XMVectorAdd(targetPos, offset),
				XMVectorClamp
				(
					XMVectorMultiply(forward, XMVectorSet(velocityMag, velocityMag, velocityMag, 1.f)),
					XMVectorSet(-m_MaxLookAhead, -m_MaxLookAhead, -m_MaxLookAhead, 1.f),
					XMVectorSet(m_MaxLookAhead, m_MaxLookAhead, m_MaxLookAhead, 1.f)
				)
			);

		const auto currPos = XMLoadFloat3(&GetTransform()->GetPosition());
		const auto smoothPos = XMVectorLerp(currPos, finalPos, m_Smoothing);
		GetTransform()->Translate(smoothPos);
	}
}
