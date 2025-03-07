#pragma once

class BoneObject;

class SoftwareSkinningScene_1 final : public GameScene
{
public:
	SoftwareSkinningScene_1() : GameScene(L"SoftwareSkinningScene_1") {};
	~SoftwareSkinningScene_1() override;
	SoftwareSkinningScene_1(const SoftwareSkinningScene_1& other) = delete;
	SoftwareSkinningScene_1(SoftwareSkinningScene_1&& other) noexcept = delete;
	SoftwareSkinningScene_1& operator=(const SoftwareSkinningScene_1& other) = delete;
	SoftwareSkinningScene_1& operator=(SoftwareSkinningScene_1&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void OnGUI() override;

private:
	BoneObject* m_pBone0{}, * m_pBone1{};

	float* m_RotBone0 = new float[3] {0.f, 0.f, 0.f}, * m_RotBone1 = new float[3] {0.f, 0.f, 0.f};
	bool m_AutoRotation{ false };
	int m_RotationSign{ 1 };
};

