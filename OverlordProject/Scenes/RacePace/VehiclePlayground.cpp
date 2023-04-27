#include "stdafx.h"
#include "VehiclePlayground.h"
#include "Prefabs/CubePrefab.h"

float gSteerVsForwardSpeedData[2 * 8] =
{
	//SteerAmount	to	Forward Speed
	0.0f,		0.75f,
	5.0f,		0.75f,
	30.0f,		0.125f,
	120.0f,		0.1f,
};

void VehiclePlayground::Initialize()
{
	// PHYSICS MATERIALS
	PxMaterial* pGroundPhysMat = PhysXManager::Get()->GetPhysics()->createMaterial(0.6f, 0.6f, 0.1f);

	// GROUND PLANE
	GameSceneExt::CreatePhysXGroundPlane(*this, pGroundPhysMat);

	// VEHICLE
	PxScene* currScene;
	PxGetPhysics().getScenes(&currScene, PxGetPhysics().getNbScenes(), 0);
	PxInitVehicleSDK(PxGetPhysics());
	PxVehicleSetBasisVectors(PxVec3(0, 1, 0), PxVec3(0, 0, 1));
}

VehiclePlayground::~VehiclePlayground()
{
	PxCloseVehicleSDK();
}

void VehiclePlayground::Update()
{
	
}

void VehiclePlayground::OnGUI()
{
}
