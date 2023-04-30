#pragma once
class FollowCamera final : public GameObject
{
public:
	FollowCamera(GameObject* pTarget, PxVehicleDrive4W* pVehicle, const XMFLOAT3& offsetDir);
	~FollowCamera() override = default;
	FollowCamera(const FollowCamera& other) = delete;
	FollowCamera(FollowCamera&& other) noexcept = delete;
	FollowCamera& operator=(const FollowCamera& other) = delete;
	FollowCamera& operator=(FollowCamera&& other) noexcept = delete;

	void SetSmoothing(const float smoothing) { m_Smoothing = smoothing; }
	void SetLookAhead(const float lookAhead) { m_MaxLookAhead = lookAhead; }

protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;

private:
	GameObject* m_pTarget{ nullptr };
	PxVehicleDrive4W* m_pVehicle{ nullptr };
	CameraComponent* m_pCamera{ nullptr };

	float m_Smoothing{ 0.25f };
	float m_MaxLookAhead{ 10.f };
	float m_OffsetDistance{ 30.f };
	XMFLOAT3 m_OffsetDir{ 0.f, 0.f, 0.f };
};