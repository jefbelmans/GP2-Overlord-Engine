#pragma once
class FollowCamera final : public GameObject
{
public:
	FollowCamera(GameObject* pTarget, PxVehicleTelemetryData* pTelemetryData, const XMFLOAT3& offset);
	~FollowCamera() override = default;
	FollowCamera(const FollowCamera& other) = delete;
	FollowCamera(FollowCamera&& other) noexcept = delete;
	FollowCamera& operator=(const FollowCamera& other) = delete;
	FollowCamera& operator=(FollowCamera&& other) noexcept = delete;

	void SetSmoothing(const float smoothing) { m_Smoothing = smoothing; }

protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;

private:
	GameObject* m_pTarget{ nullptr };
	PxVehicleTelemetryData* m_pTelemetryData{ nullptr };
	CameraComponent* m_pCamera{ nullptr };

	float m_Smoothing{ 0.25f };
	float m_MaxLookAhead{ 10.f };
	XMFLOAT3 m_Offset{ 0.f, 0.f, 0.f };
};