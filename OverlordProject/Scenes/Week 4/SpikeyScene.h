#pragma once
class SpikeyMaterial;
class SpikeyScene final : public GameScene
{
public:
	SpikeyScene();
	~SpikeyScene() override = default;

	SpikeyScene(const SpikeyScene& other) = delete;
	SpikeyScene(SpikeyScene&& other) noexcept = delete;
	SpikeyScene& operator=(const SpikeyScene& other) = delete;
	SpikeyScene& operator=(SpikeyScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void OnGUI() override;

private:
	GameObject* m_pSpikeySphere{ nullptr };
	SpikeyMaterial* m_pSpikeyMaterial{ nullptr };
	float m_RotationAngle{0.f};
};

