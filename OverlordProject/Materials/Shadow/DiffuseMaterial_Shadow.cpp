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

void DiffuseMaterial_Shadow::InitializeEffectVariables()
{
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
			XMLoadFloat4x4(&pShadowMapRenderer->GetLightVP()),
			XMLoadFloat4x4(&pModel->GetTransform()->GetWorld())
		));

	SetVariable_Matrix(L"gWorldViewProj_Light", lWvp);
	SetVariable_Texture(L"gShadowMap", pShadowMapRenderer->GetShadowMap());
	SetVariable_Vector(L"gLightDirection", sceneContext.pLights->GetDirectionalLight().direction);
}
