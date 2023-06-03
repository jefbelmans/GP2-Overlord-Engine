#include "stdafx.h"
#include "ButtonComponent.h"

ButtonComponent::ButtonComponent(std::function<void()> onPressed, const std::wstring& assetPath, const XMFLOAT2& position, const XMFLOAT2& scale)
	: IInteractable(position, {0.f, 0.f}, scale)
	, m_OnPressed(onPressed)
	, m_BaseAssetPath(assetPath)
	, m_SelectedAssetPath(assetPath)
	, m_SelectedColor({ .95f, .95f, .95f, 1.f })
	, m_PressedColor({ .92f, .92f, .92f, 1.f })
{}

void ButtonComponent::OnClickBegin()
{
	m_OnPressed();
	m_pSpriteComponent->SetTexture(m_PressedAssetPath);
	m_pSpriteComponent->SetColor(m_PressedColor);
}

void ButtonComponent::OnClickEnd()
{
	if (m_IsSelected)
	{
		m_pSpriteComponent->SetTexture(m_SelectedAssetPath);
		m_pSpriteComponent->SetColor(m_SelectedColor);
	}
	else
	{
		m_pSpriteComponent->SetTexture(m_BaseAssetPath);
		m_pSpriteComponent->SetColor(m_BaseColor);
	}
		
}

void ButtonComponent::OnHoverBegin()
{
	m_pSpriteComponent->SetTexture(m_SelectedAssetPath);
	m_pSpriteComponent->SetColor(m_SelectedColor);
	m_IsSelected = true;
}

void ButtonComponent::OnHoverEnd()
{
	m_pSpriteComponent->SetTexture(m_BaseAssetPath);
	m_pSpriteComponent->SetColor(m_BaseColor);
	m_IsSelected = false;
}

bool ButtonComponent::GetIsActive() const
{
	return m_pGameObject->GetIsActive();
}

void ButtonComponent::Initialize(const SceneContext&)
{
	// REGISTER WITH EVENT SYSTEM
	// EventSystem::Get()->RegisterInteractable(this);

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

	transfom->Translate(pos.x, pos.y, 1.f);
	transfom->Scale(scale.x, scale.y, 1.f);
}