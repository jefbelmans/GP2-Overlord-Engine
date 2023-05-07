#include "stdafx.h"
#include <numeric>

#include "VehiclePlayground.h"
#include "Materials/DiffuseMaterial.h"

float gSteerVsForwardSpeedData[2 * 8] =
{
	//SteerAmount	to	Forward Speed
	0.0f,		1.f,
	5.f,		0.8f,
	10.f,		0.6f,
	15.f,		0.5f,
	22.5f,		0.38f,
	30.f,		0.24f,
	PX_MAX_F32,		PX_MAX_F32,
	PX_MAX_F32,		PX_MAX_F32
};

void VehiclePlayground::Initialize()
{
	// SCENE SETTINGS
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.enableOnGUI = true;

	// MATERIALS
	DiffuseMaterial* pVehicleMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pVehicleMat->SetDiffuseTexture(L"Textures/F1_Car.png");

	DiffuseMaterial* pTrackMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pTrackMat->SetDiffuseTexture(L"Textures/F1_Track.png");

	// GROUND PLANE
	// GameSceneExt::CreatePhysXGroundPlane(*this, PhysXManager::Get()->GetPhysics()->createMaterial(0.5f, 0.5f, 0.f));

	// PHYSX DEBUG RENDERING
	GetPhysxProxy()->EnablePhysxDebugRendering(true);
	GetPhysxProxy()->GetPhysxScene()->setVisualizationParameter(PxVisualizationParameter::eBODY_LIN_VELOCITY, 1.f);
	
	// TRACK
	m_pTrack = new GameObject();
	m_pTrack->AddComponent(new ModelComponent(L"Meshes/F1_Track.ovm"))->SetMaterial(pTrackMat);
	m_pTrack->GetTransform()->Translate(0.f, -1.f, 0.f);
	AddChild(m_pTrack);

	// INIT VEHICLE SDK
	m_pVehicle = PhysXManager::Get()->InitializeVehicleSDK();
	m_pVehicleInputData = new PxVehicleDrive4WRawInputData();
	m_SteerVsForwardSpeedTable = PxFixedSizeLookupTable<8>(gSteerVsForwardSpeedData, 6);

	// CHASIS
	m_pChassis = new GameObject();
	AddChild(m_pChassis);

	m_pChassis->GetTransform()->Translate(XMFLOAT3{ 0.f, 4.f, 0.f });
	m_pChassis->AddComponent(new ModelComponent(L"Meshes/F1_Car.ovm"))->SetMaterial(pVehicleMat);

	auto pRb = m_pChassis->AddComponent(new RigidBodyComponent());
	// SET VEHICLE RIGID ACTOR TO RB RIGID ACTOR
	pRb->SetPxRigidActor(m_pVehicle->getRigidDynamicActor());

	// WHEELS
	for (int i = 0; i < 4; i++)
	{
		m_pWheels[i] = new GameObject();
		AddChild(m_pWheels[i]);
		m_pWheels[i]->AddComponent(new ModelComponent(L"Meshes/F1_Wheel.ovm"))->SetMaterial(pVehicleMat);
	}

	auto testBox = new GameObject();
	pRb = testBox->AddComponent(new RigidBodyComponent());
	pRb->AddCollider(PxBoxGeometry{ 1.f,1.f,1.f }, *PhysXManager::Get()->GetPhysics()->createMaterial(0.5f, 0.5f, 1.f));
	testBox->GetTransform()->Translate(0.f, 10.f, 2.f);
	AddChild(testBox);

	SetupTelemetryData();

	// CAMERA
	m_pCamera = AddChild(new FollowCamera(m_pChassis, m_pVehicle, m_CameraPitch));
	m_pCamera->GetComponent<CameraComponent>()->SetActive(true);
	m_pCamera->SetSmoothing(m_CameraSmoothing);
	m_pCamera->SetLookAhead(m_CameraLookAhead);
	m_pCamera->SetOffsetDistance(m_CameraDistance);

	//Input
	auto inputAction = InputAction(SteerLeft, InputState::down, VK_LEFT);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(SteerRight, InputState::down, VK_RIGHT);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(Accelerate, InputState::down, VK_UP);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(Deaccelerate, InputState::down, VK_DOWN);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(HandBrake, InputState::down, VK_SPACE, -1, XINPUT_GAMEPAD_X);
	m_SceneContext.pInput->AddInputAction(inputAction);
}

VehiclePlayground::~VehiclePlayground()
{
	PxCloseVehicleSDK();
}

void VehiclePlayground::Update()
{
	UpdateVehicle();
	UpdateInput();
}

void VehiclePlayground::OnGUI()
{
	// CAMERA SETTINGS
	if(ImGui::CollapsingHeader("Camera Settings"))
	{
		ImGui::SliderFloat("Pitch", &m_CameraPitch, 0.f, 90.f);
		// m_pCamera->SetPitch(m_CameraPitch);

		ImGui::SliderFloat("Smoothing", &m_CameraSmoothing, 0.f, 1.f);
		m_pCamera->SetSmoothing(m_CameraSmoothing);

		ImGui::SliderFloat("LookAhead", &m_CameraLookAhead, 0.f, 100.f);
		m_pCamera->SetLookAhead(m_CameraLookAhead);

		ImGui::SliderFloat("Offset Distance", &m_CameraDistance, 0.f, 100.f);
		m_pCamera->SetOffsetDistance(m_CameraDistance);
	}

	float xy[2 * PxVehicleGraph::eMAX_NB_SAMPLES];
	PxVec3 color[PxVehicleGraph::eMAX_NB_SAMPLES];
	char title[PxVehicleGraph::eMAX_NB_TITLE_CHARS];
	m_pVehicleTelemetryData->getEngineGraph().computeGraphChannel(PxVehicleDriveGraphChannel::eENGINE_REVS,
		xy, color, title);

	
	// CAR TELEMETRY
	{
		ImGui::Text("Car Telemetry");
		ImGui::Text("Speed: %f", m_pVehicle->computeForwardSpeed());
		ImGui::Text("Lateral Slip: %f %f %f %f", 
			m_WheelQueryResults[0].lateralSlip,
			m_WheelQueryResults[1].lateralSlip,
			m_WheelQueryResults[2].lateralSlip,
			m_WheelQueryResults[3].lateralSlip);
	}
}

void VehiclePlayground::SetupTelemetryData()
{
	m_pVehicleTelemetryData = PxVehicleTelemetryData::allocate(4);

	const PxF32 graphSizeX = 0.25f;
	const PxF32 graphSizeY = 0.25f;
	const PxF32 engineGraphPosX = 0.5f;
	const PxF32 engineGraphPosY = 0.5f;
	const PxF32 wheelGraphPosX[4] = { 0.75f,0.25f,0.75f,0.25f };
	const PxF32 wheelGraphPosY[4] = { 0.75f,0.75f,0.25f,0.25f };
	const PxVec3 backgroundColor(255, 255, 255);
	const PxVec3 lineColorHigh(255, 0, 0);
	const PxVec3 lineColorLow(0, 0, 0);
	m_pVehicleTelemetryData->setup
	(graphSizeX, graphSizeY,
		engineGraphPosX, engineGraphPosY,
		wheelGraphPosX, wheelGraphPosY,
		backgroundColor, lineColorHigh, lineColorLow);
}

void VehiclePlayground::UpdateInput()
{
	ReleaseAllControls();

	auto input{ m_SceneContext.pInput };
	
	// Vibrations
	const float avgLateralSlip = std::accumulate(m_WheelQueryResults, m_WheelQueryResults + 4, 0.0f,
		[](float sum, const physx::PxWheelQueryResult& wheelQueryResult) {
			return sum + std::abs(wheelQueryResult.lateralSlip);
		}) / 4.f;

	input->SetVibration(avgLateralSlip * 0.03f, avgLateralSlip * 0.085f);

	float steerInput = input->GetThumbstickPosition(true).x;
	float throtleInput = input->GetTriggerPressure(false);
	float reverseThrotleInput = input->GetTriggerPressure(true);

	if (steerInput != 0.f)
		Steer(steerInput);

	if(throtleInput != 0.f)
		AccelerateForward(throtleInput);

	if (reverseThrotleInput != 0.f)
		AccelerateReverse(reverseThrotleInput);

	if (input->IsActionTriggered(HandBrake))
		Handbrake();

	if (input->IsActionTriggered(SteerLeft))
		Steer(-1.f);
	else if (input->IsActionTriggered(SteerRight))
		Steer(1.f);

	if (input->IsActionTriggered(Accelerate))
		AccelerateForward(1.f);
	else if (input->IsActionTriggered(Deaccelerate))
		AccelerateReverse(1.f);
}

void VehiclePlayground::UpdateVehicle()
{
	if (m_pVehicle == nullptr) return;

	vehicle::VehicleSceneQueryData* vehicleSceneQueryData = PhysXManager::Get()->GetVehicleSceneQueryData();
	PxBatchQuery* batchQuery = PhysXManager::Get()->GetBatchQuery();
	auto tireFrictionPairs = PhysXManager::Get()->GetFrictionPairs();

	//Update the control inputs for the vehicle
	if (m_IsDigitalControl)
		PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(m_keySmoothingData, m_SteerVsForwardSpeedTable,
			*m_pVehicleInputData, m_SceneContext.pGameTime->GetElapsed(), m_IsVehicleInAir, *m_pVehicle);
	else
		PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(m_padSmoothingData, m_SteerVsForwardSpeedTable,
			*m_pVehicleInputData, m_SceneContext.pGameTime->GetElapsed(), m_IsVehicleInAir, *m_pVehicle);

	//Raycasts
	PxVehicleWheels* vehicleWheels[6] = { m_pVehicle };
	PxRaycastQueryResult* raycastResults = vehicleSceneQueryData->getRaycastQueryResultBuffer(0);
	const PxU32 raycastResultsSize = vehicleSceneQueryData->getQueryResultBufferSize();
	PxVehicleSuspensionRaycasts(batchQuery, 1, vehicleWheels, raycastResultsSize, raycastResults);

	//Vehicle update
	const PxVec3 grav = GetPhysxProxy()->GetPhysxScene()->getGravity();
	PxVehicleWheelQueryResult vehicleQueryResults[1] = { {m_WheelQueryResults, m_pVehicle->mWheelsSimData.getNbWheels()} };

	PxVehicleUpdateSingleVehicleAndStoreTelemetryData(m_SceneContext.pGameTime->GetElapsed(), grav,
		*tireFrictionPairs, m_pVehicle, vehicleQueryResults, *m_pVehicleTelemetryData);

	//Work out if the vehicle is in the air.
	m_IsVehicleInAir = m_pVehicle->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]);

	PxShape* shapes[5];
	m_pVehicle->getRigidDynamicActor()->getShapes(shapes, 5);

	// Translate and rotate m_pWheelFL gameobject to match with shape of the wheel attached to rigid body of m_pVehicle
	for (int i = 0; i < 4; i++)
	{
		PxTransform localTm = shapes[i]->getLocalPose();
		PxMat44 shapePose = PxMat44(localTm);
		PxMat44 vehiclePose = PxMat44(m_pVehicle->getRigidDynamicActor()->getGlobalPose());
		PxMat44 wheelPose = vehiclePose * shapePose;

		auto wheelPos = wheelPose.getPosition();
		m_pWheels[i]->GetTransform()->Translate(wheelPos.x, wheelPos.y, wheelPos.z);

		auto wheelRot = PxQuat(PxMat33(wheelPose.getBasis(0), wheelPose.getBasis(1), wheelPose.getBasis(2)));

		// Left wheels are facing inwards, rotate them 180 degrees
		if (i == 0 || i == 2)
		{
			PxQuat rotate180(PxPi, PxVec3(0, 1, 0));
			wheelRot = wheelRot * rotate180;
		}

		m_pWheels[i]->GetTransform()->Rotate(XMVectorSet(wheelRot.x, wheelRot.y, wheelRot.z, wheelRot.w));
	}

}

void VehiclePlayground::AccelerateForward(float analogAcc)
{
	//If I am going reverse, change to first gear
	if (m_pVehicle->mDriveDynData.getCurrentGear() == PxVehicleGearsData::eREVERSE)
	{
		m_pVehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	}

	if (m_IsDigitalControl)
	{
		m_pVehicleInputData->setDigitalAccel(true);
	}
	else
	{
		m_pVehicleInputData->setAnalogAccel(analogAcc);
	}
}

void VehiclePlayground::AccelerateReverse(float analogAcc /*= 0.f*/)
{
	//Force gear change to reverse
	m_pVehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);

	if (m_IsDigitalControl)
	{
		m_pVehicleInputData->setDigitalAccel(true);
	}
	else
	{
		m_pVehicleInputData->setAnalogAccel(analogAcc);
	}
}

void VehiclePlayground::Brake()
{
	if (m_IsDigitalControl)
	{
		m_pVehicleInputData->setDigitalBrake(true);
	}
	else
	{
		m_pVehicleInputData->setAnalogBrake(1.f);
	}
}

void VehiclePlayground::Steer(float analogSteer /*= 0.f*/)
{
	if (m_IsDigitalControl)
	{
		if (analogSteer <= 0.f)
			m_pVehicleInputData->setDigitalSteerLeft(true);
		else
			m_pVehicleInputData->setDigitalSteerRight(true);
	}
	else
	{
		m_pVehicleInputData->setAnalogSteer(analogSteer);
	}
}

void VehiclePlayground::Handbrake()
{
	if (m_IsDigitalControl)
	{
		m_pVehicleInputData->setDigitalHandbrake(true);
	}
	else
	{
		m_pVehicleInputData->setAnalogHandbrake(1.f);
	}
}

void VehiclePlayground::ReleaseAllControls()
{
	if (m_IsDigitalControl)
	{
		m_pVehicleInputData->setDigitalAccel(false);
		m_pVehicleInputData->setDigitalSteerLeft(false);
		m_pVehicleInputData->setDigitalSteerRight(false);
		m_pVehicleInputData->setDigitalBrake(false);
		m_pVehicleInputData->setDigitalHandbrake(false);
	}
	else
	{
		m_pVehicleInputData->setAnalogAccel(0.0f);
		m_pVehicleInputData->setAnalogSteer(0.0f);
		m_pVehicleInputData->setAnalogBrake(0.0f);
		m_pVehicleInputData->setAnalogHandbrake(0.0f);
	}
}