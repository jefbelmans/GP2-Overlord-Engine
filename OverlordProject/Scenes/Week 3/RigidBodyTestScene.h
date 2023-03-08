#pragma once
class RigidBodyTestScene final : public GameScene
{
public:
	RigidBodyTestScene() : GameScene(L"Rigid Body Test Scene") {};
	~RigidBodyTestScene() override = default;

	RigidBodyTestScene(const RigidBodyTestScene& other) = delete;
	RigidBodyTestScene(RigidBodyTestScene&& other) noexcept = delete;
	RigidBodyTestScene& operator=(const RigidBodyTestScene& other) = delete;
	RigidBodyTestScene& operator=(RigidBodyTestScene&& other) noexcept = delete;

protected:
	void Initialize() override;
};

