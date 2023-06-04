#include "stdafx.h"
#include "PostMotionBlur.h"

PostMotionBlur::PostMotionBlur() :
	PostProcessingMaterial(L"Effects/Post/MotionBlur.fx")
{}

void PostMotionBlur::UpdateBaseEffectVariables(const SceneContext& sceneContext, RenderTarget* pSource)
{
	// Set Source Texture
	const auto pSourceSRV = pSource->GetColorShaderResourceView();
	m_pBaseEffect->GetVariableByName("gColorTexture")->AsShaderResource()->SetResource(pSourceSRV);

	// Set Depth Texture
	const auto pDepthSRV = pSource->GetDepthShaderResourceView();
	m_pBaseEffect->GetVariableByName("gDepthTexture")->AsShaderResource()->SetResource(pDepthSRV);

	// Only supported in deferred
	if (sceneContext.useDeferredRendering)
	{
		// Set Mask Texture
		const auto pMaskSRV = DeferredRenderer::Get()->GetRenderTarget(DeferredRenderer::eGBufferId::Mask)->GetColorShaderResourceView();
		m_pBaseEffect->GetVariableByName("gMaskTexture")->AsShaderResource()->SetResource(pMaskSRV);
	}

	// Set ViewProjection Inverse Matrix
	m_pBaseEffect->GetVariableByName("gInverseViewProj")->AsMatrix()->SetMatrix(&(sceneContext.pCamera->GetViewProjectionInverse())._11);

	// Set Previous ViewProjection Matrix
	m_pBaseEffect->GetVariableByName("gPreviousViewProj")->AsMatrix()->SetMatrix(&m_PreviousViewProjection._11);

	// Cache Current ViewProjection Matrix
	m_PreviousViewProjection = sceneContext.pCamera->GetViewProjection();
}