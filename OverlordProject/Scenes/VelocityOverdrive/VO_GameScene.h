#pragma once

class PostMotionBlur;
class VO_GameScene final : public GameScene
{
public:
	VO_GameScene() : GameScene(L"Velocity Overdrive - Game Scene") {};
	~VO_GameScene() override;
	VO_GameScene(const VO_GameScene& other) = delete;
	VO_GameScene(VO_GameScene&& other) noexcept = delete;
	VO_GameScene& operator=(const VO_GameScene& other) = delete;
	VO_GameScene& operator=(VO_GameScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void PostDraw() override;
	void OnGUI() override;

	void OnSceneActivated() override;
	void OnSceneDeactivated() override;
private:
	
#pragma region Game Settings
	TimerComponent* m_pTimer{ nullptr };
	bool m_IsPaused{ false };
	float m_RumbleStrength{};
#pragma endregion

#pragma region UI
	SpriteFont* m_pFont32{};
	SpriteFont* m_pFont24{};

	GameObject* m_pBannerLap{};
	GameObject* m_pBannerBest{};

	GameObject* m_pControllerLayout{};

	GameObject* m_pPausePanel{};
	GameObject* m_pBackButton{};
	GameObject* m_pQuitButton{};
	GameObject* m_pRestartButton{};
	GameObject* m_pBakedShadowsButton{};

	bool m_DoShowControls{ true };

	void InitializeUI();
	void TogglePauseMenu();

	void RestartGame();
	void LoadMainMenu();
	void QuitGame();
#pragma endregion

#pragma region Sound Settings
	FMOD::Channel* m_pEngineChannel{ nullptr };
	FMOD::Sound* m_pEngineSound{ nullptr };

	void InitializeSound();
	void UpdateSound();
#pragma endregion

#pragma region Post Processing Settings
	PostMotionBlur* m_pPostMotionBlur;
#pragma endregion

#pragma region Particle System Settings
	ParticleEmitterComponent* m_pEmitterRL{ nullptr };
	ParticleEmitterComponent* m_pEmitterRR{ nullptr };
	ParticleEmitterSettings m_EmitterSettings{};
#pragma endregion

#pragma region Lighting Settings
	void InitializeLighting();
	void UpdateLighting();
#pragma endregion

#pragma region Shadow Settings
	bool m_DebugShadowMap{ false };
	bool m_DebugBakedShadowMap{ false };
	bool m_UseBakedShadows{ false };
	float m_ShadowMapScale{ 0.3f };
#pragma endregion

#pragma region Camera Settings
	float m_CameraSmoothing{ 1.f };
	float m_CameraLookAhead{ 45.f };
	float m_CameraDistance{ 40.f };
	float m_CameraPitch{ 40.f };
	FollowCamera* m_pCamera{ nullptr };
	FreeCamera* m_pFreeCamera{ nullptr };
	bool isFreeCamActive{ false };
#pragma endregion
	
#pragma region Vehicle Settings
	const XMFLOAT3 m_ChassisDim{ 2.09f, 1.38f, 6.37f };

	GameObject* m_pChassis{ nullptr };
	GameObject* m_pWheels[4]{ nullptr };

	GameObject* m_pTrack{ nullptr };

	PxVehicleDrive4W* m_pVehicle{ nullptr };
	PxVehicleDrive4WRawInputData* m_pVehicleInputData{ nullptr };
	PxVehicleTelemetryData* m_pVehicleTelemetryData{ nullptr };

	PxWheelQueryResult m_WheelQueryResults[PX_MAX_NB_WHEELS];

	bool m_IsVehicleInAir{ false };
	bool m_IsDigitalControl{ false };

	PxFixedSizeLookupTable<8> m_SteerVsForwardSpeedTable;
	PxVehicleKeySmoothingData m_keySmoothingData =
	{
		{
			6.0f,	//rise rate eANALOG_INPUT_ACCEL
			6.0f,	//rise rate eANALOG_INPUT_BRAKE		
			6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
			2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
			2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
		},
		{
			10.0f,	//fall rate eANALOG_INPUT_ACCEL
			10.0f,	//fall rate eANALOG_INPUT_BRAKE		
			10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
			5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
			5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
		}
	};

	PxVehiclePadSmoothingData m_padSmoothingData =
	{
{
	6.0f,	//rise rate eANALOG_INPUT_ACCEL
	6.0f,	//rise rate eANALOG_INPUT_BRAKE		
	6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
	2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
	2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
},
{
	10.0f,	//fall rate eANALOG_INPUT_ACCEL
	10.0f,	//fall rate eANALOG_INPUT_BRAKE		
	10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
	5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
	5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
}
	};
#pragma endregion

#pragma region Crowd Settings
std::vector<GameObject*> m_pCrowd{};
#pragma endregion

#pragma region Input Settings
	enum InputIds
	{
		SteerLeft,
		SteerRight,
		Accelerate,
		Deaccelerate,
		HandBrake,
		TogglePause,
		Confirm
	};
#pragma endregion

#pragma region Checkpoint Settings
	std::vector<XMFLOAT3> m_CheckpointPositions
	{
		XMFLOAT3(-55.f,		3.0f,	-96.0f),
		XMFLOAT3(-120.0f,	3.0f,	-96.0f),
		XMFLOAT3(-157.0f,	3.0f,	25.0f),
		XMFLOAT3(-55.0f,	3.0f,	55.0f),
		XMFLOAT3(-117.0f,	3.0f,	-26.0f),
		XMFLOAT3(-67.0f,	3.0f,	-35.0f),
		XMFLOAT3(75.0f,		3.0f,	-47.0f),
		XMFLOAT3(12.0f,		3.0f,	93.0f),
		XMFLOAT3(95.0f,		3.0f,	143.0f),
		XMFLOAT3(141.0f,	3.0f,	-59.0f)
	};

	std::vector<XMFLOAT3> m_CheckpointRotations
	{
		XMFLOAT3(0.f, 90.f, 0.f),
		XMFLOAT3(0.f, 90.f, 0.f),
		XMFLOAT3(0.f, 0.f, 0.f),
		XMFLOAT3(0.f, 90.f, 0.f),
		XMFLOAT3(0.f, 0.f, 0.f),
		XMFLOAT3(0.f, 90.f, 0.f),
		XMFLOAT3(0.f, 90.f, 0.f),
		XMFLOAT3(0.f, 0.f, 0.f),
		XMFLOAT3(0.f, 90.f, 0.f),
		XMFLOAT3(0.f, 0.f, 0.f)
	};

	std::vector<GameObject*> m_pCheckpoints;
	int m_NextCheckpoint{ 0 };
#pragma endregion

	void ConstructScene();

	void SetupTelemetryData();

	void UpdateInput();
	void UpdateVehicle();

	void OnTriggerCallback(GameObject* trigger, GameObject* other, PxTriggerAction action);

	void AccelerateForward(float analogAcc = 0.f);
	void AccelerateReverse(float analogAcc = 0.f);
	void Brake();

	void Steer(float analogSteer = 0.f);
	void Handbrake();

	void ReleaseAllControls();
};

