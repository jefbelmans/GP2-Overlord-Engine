#include "stdafx.h"
#include "CameraComponent.h"

CameraComponent::CameraComponent() :
	m_FarPlane(2500.0f),
	m_NearPlane(0.1f),
	m_FOV(XM_PIDIV4),
	m_Size(25.0f),
	m_PerspectiveProjection(true)
{
	XMStoreFloat4x4(&m_Projection, XMMatrixIdentity());
	XMStoreFloat4x4(&m_View, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewInverse, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjection, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjectionInverse, XMMatrixIdentity());
}

void CameraComponent::Update(const SceneContext& sceneContext)
{
	// see https://stackoverflow.com/questions/21688529/binary-directxxmvector-does-not-define-this-operator-or-a-conversion
	using namespace DirectX;

	XMMATRIX projection{};

	if (m_PerspectiveProjection)
	{
		projection = XMMatrixPerspectiveFovLH(m_FOV, sceneContext.aspectRatio, m_NearPlane, m_FarPlane);
	}
	else
	{
		const float viewWidth = (m_Size > 0) ? m_Size * sceneContext.aspectRatio : sceneContext.windowWidth;
		const float viewHeight = (m_Size > 0) ? m_Size : sceneContext.windowHeight;
		projection = XMMatrixOrthographicLH(viewWidth, viewHeight, m_NearPlane, m_FarPlane);
	}

	const XMVECTOR worldPosition = XMLoadFloat3(&GetTransform()->GetWorldPosition());
	const XMVECTOR lookAt = XMLoadFloat3(&GetTransform()->GetForward());
	const XMVECTOR upVec = XMLoadFloat3(&GetTransform()->GetUp());

	const XMMATRIX view = XMMatrixLookAtLH(worldPosition, worldPosition + lookAt, upVec);
	const XMMATRIX viewInv = XMMatrixInverse(nullptr, view);
	const XMMATRIX viewProjectionInv = XMMatrixInverse(nullptr, view * projection);

	XMStoreFloat4x4(&m_Projection, projection);
	XMStoreFloat4x4(&m_View, view);
	XMStoreFloat4x4(&m_ViewInverse, viewInv);
	XMStoreFloat4x4(&m_ViewProjection, view * projection);
	XMStoreFloat4x4(&m_ViewProjectionInverse, viewProjectionInv);
}

void CameraComponent::SetActive(bool active)
{
	if (m_IsActive == active) return;

	const auto pGameObject = GetGameObject();
	ASSERT_IF(!pGameObject, L"Failed to set active camera. Parent game object is null");

	if (!pGameObject) return; //help the compiler... (C6011)
	const auto pScene = pGameObject->GetScene();
	ASSERT_IF(!pScene, L"Failed to set active camera. Parent game scene is null");

	m_IsActive = active;
	pScene->SetActiveCamera(active?this:nullptr); //Switch to default camera if active==false
}

GameObject* CameraComponent::Pick(CollisionGroup ignoreGroups) const
{
	// CONVERT MOUSE POS TO NDC
	const auto halfWidth = m_pScene->GetSceneContext().windowWidth / 2.0f;
	const auto halfHeight = m_pScene->GetSceneContext().windowHeight / 2.0f;

	const auto mousePos = m_pScene->GetSceneContext().pInput->GetMousePosition();
	const auto ndc = DirectX::XMFLOAT2{ (mousePos.x - halfWidth) / halfWidth, (halfHeight - mousePos.y) / halfHeight};

	// CALCULATE NEAR AND FAR POINT
	const auto ndcn = DirectX::XMFLOAT4{ ndc.x, ndc.y, 0.0f, 0.0f };
	const auto ndcf = DirectX::XMFLOAT4{ ndc.x, ndc.y, 1.0f, 0.0f };

	XMFLOAT4 nearPoint;
	DirectX::XMStoreFloat4(&nearPoint, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat4(&ndcn), DirectX::XMLoadFloat4x4(&m_ViewProjectionInverse)));
	XMFLOAT4 farPoint;
	DirectX::XMStoreFloat4(&farPoint, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat4(&ndcf), DirectX::XMLoadFloat4x4(&m_ViewProjectionInverse)));

	// RAYCAST
	PxQueryFilterData filterData{};
	filterData.data.word0 = ~UINT(ignoreGroups);

	const PxVec3 rayStart = PxVec3(nearPoint.x, nearPoint.y, nearPoint.z);
	const PxVec3 rayDir = PxVec3(farPoint.x - nearPoint.x, farPoint.y - nearPoint.y, farPoint.z - nearPoint.z).getNormalized();

	PxRaycastBuffer hit{};
	if (m_pScene->GetPhysxProxy()->Raycast(rayStart, rayDir, PX_MAX_F32, hit, PxHitFlag::eDEFAULT, filterData))
	{
		if (hit.hasBlock && hit.block.actor->userData)
		{
			Logger::LogDebug(L"Hit something!");
			return reinterpret_cast<BaseComponent*>(hit.block.actor->userData)->GetGameObject();
		}
	}
	return nullptr;
}