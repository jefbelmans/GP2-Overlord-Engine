#pragma once
#include "PxPhysicsAPI.h"

class VehiclePlayground final : public GameScene
{
public:
	VehiclePlayground() : GameScene(L"Vehicle Playground") {};
	~VehiclePlayground() override;
	VehiclePlayground(const VehiclePlayground& other) = delete;
	VehiclePlayground(VehiclePlayground&& other) noexcept = delete;
	VehiclePlayground& operator=(const VehiclePlayground& other) = delete;
	VehiclePlayground& operator=(VehiclePlayground&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void OnGUI() override;

private:
	enum InputIds
	{
		SteerLeft,
		SteerRight,
		Accelerate,
		Deaccelerate,
		HandBrake
	};

	float m_CameraSmoothing{ 0.35f };
	FollowCamera* m_pCamera{ nullptr };

	GameObject* m_pChassis{ nullptr };

	PxVehicleDrive4W* m_pVehicle{ nullptr };
	PxVehicleDrive4WRawInputData* m_pVehicleInputData{ nullptr };
	PxVehicleTelemetryData* m_pVehicleTelemetryData{ nullptr };

	bool m_IsVehicleInAir{ false };
	bool m_IsDigitalControl{ true };

	PxFixedSizeLookupTable<8>			m_SteerVsForwardSpeedTable;
	PxVehicleKeySmoothingData			m_keySmoothingData =
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
	PxVehiclePadSmoothingData			m_padSmoothingData =
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

	void SetupTelemetryData();

	void UpdateInput();
	void UpdateVehicle();

	void AccelerateForward(float analogAcc = 0.f);
	void AccelerateReverse(float analogAcc = 0.f);
	void Brake();

	void Steer(float analogSteer = 0.f);
	void Handbrake();

	void ReleaseAllControls();
};

