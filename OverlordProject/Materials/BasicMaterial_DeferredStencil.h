#pragma once

class BasicMaterial_DeferredStencil : public Material<BasicMaterial_DeferredStencil>
{
public:
	BasicMaterial_DeferredStencil();
	~BasicMaterial_DeferredStencil() override = default;

	BasicMaterial_DeferredStencil(const BasicMaterial_DeferredStencil& other) = delete;
	BasicMaterial_DeferredStencil(BasicMaterial_DeferredStencil&& other) noexcept = delete;
	BasicMaterial_DeferredStencil& operator=(const BasicMaterial_DeferredStencil& other) = delete;
	BasicMaterial_DeferredStencil& operator=(BasicMaterial_DeferredStencil&& other) noexcept = delete;

	void SetDiffuseMap(const std::wstring& assetFile);
	void SetDiffuseMap(TextureData* pTextureData);

	void SetNormalMap(const std::wstring& assetFile);
	void SetNormalMap(TextureData* pTextureData);

	void SetSpecularMap(const std::wstring& assetFile);
	void SetSpecularMap(TextureData* pTextureData);

	void UseTransparency(bool enable);

protected:
	void InitializeEffectVariables() override;
	void OnUpdateModelVariables(const SceneContext& /*sceneContext*/, const ModelComponent* /*pModel*/) const;
};

