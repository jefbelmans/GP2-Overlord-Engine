#include "stdafx.h"
#include "LightManager.h"

LightManager::LightManager()
{
	m_DirectionalLight.direction = { -0.577f, -0.577f, 0.577f , 1.0f };
	m_DirectionalLight.color = { 0.9f, 0.9f, 0.8f, 1.0f };

	m_BakedDirectionalLight.direction = { -0.577f, -0.577f, 0.577f , 1.0f };
}

LightManager::~LightManager()
{
	m_Lights.clear();
}

void LightManager::SetDirectionalLight(const XMFLOAT3& position, const XMFLOAT3& direction)
{
	m_DirectionalLight.direction = { direction.x, direction.y, direction.z , 1.0f };
	m_DirectionalLight.position = { position.x, position.y, position.z , 1.0f };
	m_DirectionalLight.isDirty = true;
}

void LightManager::SetBakedDirectionalLight(const XMFLOAT3& position, const XMFLOAT3& direction)
{
	m_BakedDirectionalLight.direction = { direction.x, direction.y, direction.z , 1.0f };
	m_BakedDirectionalLight.position = { position.x, position.y, position.z , 1.0f };
	m_BakedDirectionalLight.isDirty = true;
}

void LightManager::SetUseBakedShadows(bool useBaked)
{
	m_UseBakedShadows = useBaked;
	if(useBaked)
		m_DoBakeShadows = !ShadowMapRenderer::Get()->GetIsBakedShadowMapInitialized();
}