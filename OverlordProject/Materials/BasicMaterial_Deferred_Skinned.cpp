#include "stdafx.h"
#include "BasicMaterial_Deferred_Skinned.h"

BasicMaterial_Deferred_Skinned::BasicMaterial_Deferred_Skinned()
	: Material(L"Effects/Deferred/BasicEffect_Deferred_Skinned.fx")
{}


void BasicMaterial_Deferred_Skinned::SetDiffuseMap(const std::wstring& assetFile)
{
	SetDiffuseMap(ContentManager::Load<TextureData>(assetFile));
}

void BasicMaterial_Deferred_Skinned::SetDiffuseMap(TextureData* pTextureData)
{
	SetVariable_Scalar(L"gUseDiffuseMap", pTextureData != nullptr);
	SetVariable_Texture(L"gDiffuseMap", pTextureData);
}

void BasicMaterial_Deferred_Skinned::SetNormalMap(const std::wstring& assetFile)
{
	SetNormalMap(ContentManager::Load<TextureData>(assetFile));
}

void BasicMaterial_Deferred_Skinned::SetNormalMap(TextureData* pTextureData)
{
	SetVariable_Scalar(L"gUseNormalMap", pTextureData != nullptr);
	SetVariable_Texture(L"gNormalMap", pTextureData);
}

void BasicMaterial_Deferred_Skinned::SetSpecularMap(const std::wstring& assetFile)
{
	SetSpecularMap(ContentManager::Load<TextureData>(assetFile));

}

void BasicMaterial_Deferred_Skinned::SetSpecularMap(TextureData* pTextureData)
{
	SetVariable_Scalar(L"gUseSpecularMap", pTextureData != nullptr);
	SetVariable_Texture(L"gSpecularMap", pTextureData);
}

void BasicMaterial_Deferred_Skinned::UseTransparency(bool /*enable*/)
{
	//SetTechnique(enable ? L"Default_Alpha" : L"Default");
}

void BasicMaterial_Deferred_Skinned::WriteToMask(bool enable)
{
	SetVariable_Scalar(L"gWriteToMask", enable);
}

void BasicMaterial_Deferred_Skinned::InitializeEffectVariables()
{

}

void BasicMaterial_Deferred_Skinned::OnUpdateModelVariables(const SceneContext& sceneContext, const ModelComponent* pModel) const
{
	SetVariable_Vector(L"gLightDirection", sceneContext.pLights->GetDirectionalLight().direction);

	auto anim = pModel->GetAnimator();
	ASSERT_NULL_(anim);
	auto boneTransforms = anim->GetBoneTransforms();
	SetVariable_MatrixArray(L"gBones", &boneTransforms[0]._11, (UINT)boneTransforms.size());
}