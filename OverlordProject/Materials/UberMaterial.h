#pragma once
class UberMaterial final : public Material<UberMaterial>
{
public:
	UberMaterial();
	~UberMaterial() override = default;

	UberMaterial(const UberMaterial& other) = delete;
	UberMaterial(UberMaterial&& other) noexcept = delete;
	UberMaterial& operator=(const UberMaterial& other) = delete;
	UberMaterial& operator=(UberMaterial&& other) noexcept = delete;

protected:
	void InitializeEffectVariables() override;

private:
	// TEXTURES
	TextureData* m_pDiffuseTexture{ nullptr };
	TextureData* m_pNormalTexture{ nullptr };
	TextureData* m_pEnvironmentTexture{ nullptr };
};

