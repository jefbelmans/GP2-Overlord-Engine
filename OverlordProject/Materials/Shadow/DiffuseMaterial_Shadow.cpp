#include "stdafx.h"
#include "DiffuseMaterial_Shadow.h"

DiffuseMaterial_Shadow::DiffuseMaterial_Shadow():
	Material(L"Effects/Shadow/PosNormTex3D_Shadow.fx")
{}

void DiffuseMaterial_Shadow::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture);
}

void DiffuseMaterial_Shadow::SetDiffuseColor(const XMFLOAT4& color)
{
	SetVariable_Vector(L"gDiffuseColor", color);
}

void DiffuseMaterial_Shadow::UseDiffuseMap(bool useMap)
{
	SetVariable_Scalar(L"gUseDiffuseMap", useMap);
}

void DiffuseMaterial_Shadow::InitializeEffectVariables()
{
	const auto pShadowMapRenderer = ShadowMapRenderer::Get();
	SetVariable_Texture(L"gBakedShadowMap", pShadowMapRenderer->GetBakedShadowMap());
}

void DiffuseMaterial_Shadow::OnUpdateModelVariables(const SceneContext& sceneContext, const ModelComponent* pModel) const
{
	/*
	 * TODO_W8
	 * Update The Shader Variables
	 * 1. Update the LightWVP > Used to Transform a vertex into Light clipping space
	 * 	LightWVP = model_world * light_viewprojection
	 * 	(light_viewprojection [LightVP] can be acquired from the ShadowMapRenderer)
	 *
	 * 2. Update the ShadowMap texture
	 *
	 * 3. Update the Light Direction (retrieve the direction from the LightManager > sceneContext)
	*/

	//Update Shadow Variables
	const auto pShadowMapRenderer = ShadowMapRenderer::Get();

	XMFLOAT4X4 lWvp;
	XMStoreFloat4x4(&lWvp, 
		XMMatrixMultiply(
			XMLoadFloat4x4(&pModel->GetTransform()->GetWorld()),
			XMLoadFloat4x4(&pShadowMapRenderer->GetLightVP())
		));

	SetVariable_Texture(L"gShadowMap", pShadowMapRenderer->GetShadowMap());
	SetVariable_Matrix(L"gWorldViewProj_Light", reinterpret_cast<const float*>(&lWvp));
	SetVariable_Vector(L"gLightDirection", sceneContext.pLights->GetDirectionalLight().direction);
	
	// BAKED SHADOWS
	SetVariable_Scalar(L"gUseBakedShadows", sceneContext.pLights->GetUseBakedShadows());
	XMStoreFloat4x4(&m_BakedLightWVP,
		XMMatrixMultiply(
			XMLoadFloat4x4(&pModel->GetTransform()->GetWorld()),
			XMLoadFloat4x4(&pShadowMapRenderer->GetBakedLightVP())
		));

	SetVariable_Matrix(L"gBakedWorldViewProj_Light", reinterpret_cast<const float*>(&m_BakedLightWVP));

	// Rotates the shadows on the race track by 90 degrees for some fucking reason
	// TODO: Fix this

	//if ((m_IsBakedLightWVPDirty || !pModel->GetGameObject()->GetIsShadowMapStatic()) && sceneContext.pLights->GetUseBakedShadows())
	//{
	//	m_IsBakedLightWVPDirty = false;
	//	SetVariable_Scalar(L"gUseBakedShadows", sceneContext.pLights->GetUseBakedShadows());
	//	XMStoreFloat4x4(&m_BakedLightWVP,
	//		XMMatrixMultiply(
	//			XMLoadFloat4x4(&pModel->GetTransform()->GetWorld()),
	//			XMLoadFloat4x4(&pShadowMapRenderer->GetBakedLightVP())
	//		));

	//	SetVariable_Matrix(L"gBakedWorldViewProj_Light", reinterpret_cast<const float*>(&m_BakedLightWVP));
	// 	m_IsBakedLightWVPDirty = false;
	//}
}
