#pragma once
class SpikeyMaterial final : public Material<SpikeyMaterial>
{
public:
	SpikeyMaterial();
	~SpikeyMaterial() override = default;

	SpikeyMaterial(const SpikeyMaterial& other) = delete;
	SpikeyMaterial(SpikeyMaterial&& other) noexcept = delete;
	SpikeyMaterial& operator=(const SpikeyMaterial& other) = delete;
	SpikeyMaterial& operator=(SpikeyMaterial&& other) noexcept = delete;

protected:
	void InitializeEffectVariables() override;

};

