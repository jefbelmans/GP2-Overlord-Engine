#include "stdafx.h"
#include "FollowCamera.h"

FollowCamera::FollowCamera(GameObject* pTarget, PxVehicleDrive4W* pVehicle, const XMFLOAT3& offsetDir)
	: m_pTarget(pTarget)
	, m_pVehicle(pVehicle)
	, m_OffsetDir(offsetDir)
{}

void FollowCamera::Initialize(const SceneContext& /*sceneContext*/)
{
	m_pCamera = new CameraComponent();
	AddComponent(m_pCamera);
    XMStoreFloat3(&m_OffsetDir, XMVector3Normalize(XMLoadFloat3(&m_OffsetDir)));
}

void FollowCamera::Update(const SceneContext& /*sceneContext*/)
{
	if (m_pCamera->IsActive())
	{
		const auto targetTransform{ m_pTarget->GetTransform() };
		const auto targetPos{ XMLoadFloat3(&targetTransform->GetPosition()) };
		const auto speed{ std::clamp(std::powf(m_pVehicle->computeForwardSpeed() * 0.1f, 2.f), 0.f, 10.f) };

		const auto offset
		{ 
			XMVectorMultiply
			(XMLoadFloat3(&m_OffsetDir),
			XMVectorSet(m_OffsetDistance + speed, m_OffsetDistance + speed, m_OffsetDistance + speed, 0.f)
		)};

		const auto finalPos = targetPos + offset;

		const auto currPos = XMLoadFloat3(&GetTransform()->GetPosition());
		const auto smoothPos = XMVectorLerp(currPos, finalPos, m_Smoothing);
		GetTransform()->Translate(smoothPos);
	}
}
