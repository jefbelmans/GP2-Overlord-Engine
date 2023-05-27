#include "stdafx.h"
#include "ButtonComponent.h"

ButtonComponent::ButtonComponent(const std::wstring& assetPath, const XMFLOAT2& position, const XMFLOAT2& scale)
	: IInteractable(position, {0.f, 0.f}, scale)
	, m_BaseAssetPath(assetPath)
	, m_SelectedAssetPath(assetPath)
	, m_PressedAssetPath(assetPath)
{}

void ButtonComponent::OnClickBegin()
{
	Logger::LogInfo(L"ButtonComponent::OnClick");
	m_pSpriteComponent->SetTexture(m_PressedAssetPath);
}

void ButtonComponent::OnClickEnd()
{
	Logger::LogInfo(L"ButtonComponent::OnClickReleased");
	m_pSpriteComponent->SetTexture(m_BaseAssetPath);

	if(m_IsSelected)
		m_pSpriteComponent->SetTexture(m_SelectedAssetPath);
}

void ButtonComponent::OnHoverBegin()
{
	Logger::LogInfo(L"ButtonComponent::OnHoverBegin");
	m_pSpriteComponent->SetTexture(m_SelectedAssetPath);
	m_IsSelected = true;
}

void ButtonComponent::OnHoverEnd()
{
	Logger::LogInfo(L"ButtonComponent::OnHoverEnd");
	m_pSpriteComponent->SetTexture(m_BaseAssetPath);
	m_IsSelected = false;
}

void ButtonComponent::Initialize(const SceneContext&)
{
	// REGISTER WITH EVENT SYSTEM
	EventSystem::Get()->RegisterInteractable(this);

	// GET SPRITE COMPONENT
	m_pSpriteComponent = m_pGameObject->GetComponent<SpriteComponent>();
	if (!m_pSpriteComponent)
	{
		m_pSpriteComponent = m_pGameObject->AddComponent(new SpriteComponent(m_BaseAssetPath));
	}
	SetSize(m_pSpriteComponent->GetTexture()->GetDimension());

	// SET GAMEOBJECT TRANSFORM
	auto transfom{ GetTransform() };
	const auto& pos{ GetPosition() };
	const auto& scale{ GetScale() };

	transfom->Translate(pos.x, pos.y, 0.f);
	transfom->Scale(scale.x, scale.y, 1.f);
}