#include "stdafx.h"
#include "UberMaterial.h"

UberMaterial::UberMaterial():
	Material<UberMaterial>(L"Effects/UberShader.fx") {}

void UberMaterial::InitializeEffectVariables()
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(L"Textures/Skulls_Diffusemap.tga");
	m_pNormalTexture = ContentManager::Load<TextureData>(L"Textures/Skulls_Normalmap.tga");
	m_pEnvironmentTexture = ContentManager::Load<TextureData>(L"Textures/Cubemap.dds");
	
	// CONFIGURE EFFECT VARIABLES

	// DIFFUSE MAP
	SetVariable_Texture(L"gTextureDiffuse", m_pDiffuseTexture);
	SetVariable_Scalar(L"gUseTextureDiffuse", true);
	
	// NORMAL MAPPING
	SetVariable_Texture(L"gTextureNormal", m_pNormalTexture);
	SetVariable_Scalar(L"gUseTextureNormal", true);
	
	// ENVIRONMENT MAPPING
	SetVariable_Texture(L"gCubeEnvironment", m_pEnvironmentTexture);
	SetVariable_Scalar(L"gUseEnvironmentMapping", true);
	SetVariable_Scalar(L"gReflectionStrength", 0.7f);
	SetVariable_Scalar(L"gRefractionStrength", 0.3f);

	// SPECULAR
	SetVariable_Scalar(L"gUseSpecularBlinn", true);

	// FRESNEL
	SetVariable_Scalar(L"gUseFresnelFalloff", true);
	SetVariable_Scalar(L"gFresnelPower", 3.5f);
	SetVariable_Scalar(L"gFresnelMultiplier", 0.5f);
	SetVariable_Scalar(L"gFresnelHardness", 3.f);

}
