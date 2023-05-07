#include "stdafx.h"
#include "PhysXManager.h"

#include "PhysX/Vehicle/PhysXWheelCCDContactModifyCallback.hpp"
#include "PhysX/Vehicle/PhysXWheelContactModifyCallback.hpp"

#define PVD_HOST "127.0.0.1"
#define PVD_PORT 5425

using namespace vehicle;

void PhysXManager::Initialize()
{
	//WIN32 Allocator Implementation
	m_pDefaultAllocator = new PhysxAllocator();
	m_pDefaultErrorCallback = new PhysxErrorCallback();

	//Create Foundation
	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *m_pDefaultAllocator, *m_pDefaultErrorCallback);
	ASSERT_NULL(m_pFoundation, L"Foundation creation failed!");

#ifdef _DEBUG
	m_pPvd = PxCreatePvd(*m_pFoundation);
	if (!m_pPvd)
	{
		Logger::LogWarning(L"Physx: PhysX Visual Debugger (PVD) creation failed! (PVD connection will not be possible)");
	}
#endif

	//TODO: Fix CudaContextManager for PhysX 4.1
	////Cude context manager
	//PxCudaContextManagerDesc cudaContextManDesc = PxCudaContextManagerDesc();
	//cudaContextManDesc.interopMode = PxCudaInteropMode::D3D11_INTEROP;
	//cudaContextManDesc.graphicsDevice = pDevice;

	//m_pCudaContextManager = PxCreateCudaContextManager(*m_pFoundation, cudaContextManDesc, m_pProfileZoneManager);
	//if (m_pCudaContextManager)
	//{
	//	if (!m_pCudaContextManager->contextIsValid())
	//	{
	//		m_pCudaContextManager->release();
	//		m_pCudaContextManager = nullptr;
	//	}
	//	else
	//	{
	//		auto deviceName =std::string(m_pCudaContextManager->getDeviceName());
	//		Logger::LogFormat(LogLevel::Info, L"PhysXManager> Using Cuda Context Manager [%s]", std::wstring(deviceName.begin(), deviceName.end()).c_str());
	//	}
	//}

	//Create Main Physics Object
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true, m_pPvd);
	ASSERT_NULL(m_pPhysics, L"Physx: Physics Object creation failed!");

	PxInitExtensions(*m_pPhysics, m_pPvd);

	//Create Default CpuDispatcher (2 threads)
	m_pDefaultCpuDispatcher = PxDefaultCpuDispatcherCreate(2);

	//Try to connect with the PVD
	ToggleVisualDebuggerConnection();
} 

PhysXManager::~PhysXManager()
{
	if (m_pPhysics)
	{
		PxCloseExtensions();
	}

	if (m_pPvd && m_pPvd->getTransport())
	{
		if (m_pPvd->isConnected())
		{
			m_pPvd->getTransport()->flush();
			m_pPvd->getTransport()->disconnect();
			m_pPvd->disconnect();
		}

		m_pPvd->getTransport()->release();
	}

	PxSafeRelease(m_pPhysics);
	PxSafeRelease(m_pPvd);
	PxSafeRelease(m_pCudaContextManager);
	PxSafeRelease(m_pDefaultCpuDispatcher);
	PxSafeRelease(m_pFoundation);

	SafeDelete(m_pDefaultAllocator);
	SafeDelete(m_pDefaultErrorCallback);
}

void PhysXManager::InitializeVehicleDescription(const PxFilterData& chassisSimFilterData, const PxFilterData& wheelSimFilterData)
{
	//Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
	//The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
	//Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
	const PxF32 chassisMass = 950.f;
	const PxVec3 chassisDims(2.09f, 1.38f, 6.37f);
	const PxVec3 chassisMOI
	((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * 1.2f * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * chassisMass / 12.f);
	const PxVec3 chassisCMOffset(0.0f, 0.0f, 0.5f);

	//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
	//Moment of inertia is just the moment of inertia of a cylinder.
	const PxF32 wheelMass = 20.0f;
	const PxF32 wheelRadius = 0.422f;
	const PxF32 wheelWidth = 0.45f;
	const PxF32 wheelMOI = 0.5f * wheelMass * wheelRadius * wheelRadius;
	const PxU32 nbWheels = 4;

	m_VehicleDesc.chassisMass = chassisMass;
	m_VehicleDesc.chassisDims = chassisDims;
	m_VehicleDesc.chassisMOI = chassisMOI;
	m_VehicleDesc.chassisCMOffset = chassisCMOffset;
	m_VehicleDesc.chassisMaterial = m_pDefaultMaterial;
	m_VehicleDesc.chassisSimFilterData = chassisSimFilterData;

	m_VehicleDesc.wheelMass = wheelMass;
	m_VehicleDesc.wheelRadius = wheelRadius;
	m_VehicleDesc.wheelWidth = wheelWidth;
	m_VehicleDesc.wheelMOI = wheelMOI;
	m_VehicleDesc.numWheels = nbWheels;
	m_VehicleDesc.wheelMaterial = m_pDefaultMaterial;
	m_VehicleDesc.wheelSimFilterData = wheelSimFilterData;

	m_VehicleDesc.actorUserData = &m_ActorUserData;
	m_VehicleDesc.shapeUserDatas = &m_ShapeUserData;

}

PxScene* PhysXManager::CreateScene(GameScene* pScene)
{
	auto sceneDesc = PxSceneDesc(m_pPhysics->getTolerancesScale());
	sceneDesc.setToDefault(m_pPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = m_pDefaultCpuDispatcher;
	sceneDesc.cudaContextManager = m_pCudaContextManager;
	sceneDesc.filterShader = OverlordSimulationFilterShader;
	sceneDesc.userData = pScene;
	// sceneDesc.contactModifyCallback = &gWheelContactModifyCallback;
	// sceneDesc.ccdContactModifyCallback = &gWheelCCDContactModifyCallback;
	// sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;

	const auto physxScene = m_pPhysics->createScene(sceneDesc);
	ASSERT_IF(physxScene == nullptr, L"Scene creation failed!")

		if (m_pVehicleScene == nullptr)
			m_pVehicleScene = physxScene;

	return physxScene;
}

PxVehicleDrive4W* PhysXManager::InitializeVehicleSDK()
{
	PxInitVehicleSDK(*m_pPhysics);
	PxVehicleSetBasisVectors(PxVec3(0, 1, 0), PxVec3(0, 0, 1));
	PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

#define BLOCKING_SWEEPS
	//Create the batched scene queries for the suspension sweeps.
	//Use the post-filter shader to reject hit shapes that overlap the swept wheel at the start pose of the sweep.
	PxQueryHitType::Enum(*sceneQueryPreFilter)(PxFilterData, PxFilterData, const void*, PxU32, PxHitFlags&);
	PxQueryHitType::Enum(*sceneQueryPostFilter)(PxFilterData, PxFilterData, const void*, PxU32, const PxQueryHit&);
#ifdef BLOCKING_SWEEPS
	sceneQueryPreFilter = &WheelSceneQueryPreFilterBlocking;
	sceneQueryPostFilter = &WheelSceneQueryPostFilterBlocking;
#else
	sceneQueryPreFilter = &WheelSceneQueryPreFilterNonBlocking;
	sceneQueryPostFilter = &WheelSceneQueryPostFilterNonBlocking;
#endif 

	//Create the batched scene queries for the suspension raycasts.
	m_pVehicleSceneQueryData = VehicleSceneQueryData::allocate(m_NrOfVehicles, PX_MAX_NB_WHEELS, 1, m_NrOfVehicles, sceneQueryPreFilter, sceneQueryPostFilter, *m_pDefaultAllocator);
	m_pBatchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *m_pVehicleSceneQueryData, m_pVehicleScene);

	//Create the friction table for each combination of tire and surface type.
	m_pDefaultMaterial = m_pPhysics->createMaterial(0.5f, 0.5f, 0.1f);
	m_pFrictionPairs = createFrictionPairs(m_pDefaultMaterial);

	//Create a plane to drive on.
	PxFilterData groundPlaneSimFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0);
	auto drivePlane = createDrivablePlane(groundPlaneSimFilterData, m_pDefaultMaterial, m_pPhysics);
	m_pVehicleScene->addActor(*drivePlane);

	//Create a vehicle that will drive on the plane.
	PxFilterData chassisSimFilterData(COLLISION_FLAG_CHASSIS, COLLISION_FLAG_GROUND, 0, 0);
	PxFilterData wheelSimFilterData(COLLISION_FLAG_WHEEL, COLLISION_FLAG_WHEEL, PxPairFlag::eDETECT_CCD_CONTACT | PxPairFlag::eMODIFY_CONTACTS, 0);
	InitializeVehicleDescription(chassisSimFilterData, wheelSimFilterData);
	
	const auto pWheelMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/F1_Wheel.ovpc");
	const auto pChassisMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/F1_Car.ovpc");

	PxVehicleDrive4W* vehicleReference = createVehicle4W(m_VehicleDesc, m_pPhysics, pWheelMesh, pChassisMesh);
	PxTransform startTransform(PxVec3(0, (m_VehicleDesc.chassisDims.y * 0.5f + m_VehicleDesc.wheelRadius + 1.0f), 0), PxQuat(PxIdentity));
	vehicleReference->getRigidDynamicActor()->setGlobalPose(startTransform);
	vehicleReference->getRigidDynamicActor()->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	m_pVehicleScene->addActor(*vehicleReference->getRigidDynamicActor());

	//Set the vehicle to rest in first gear.
	//Set the vehicle to use auto-gears.
	vehicleReference->setToRestState();
	vehicleReference->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	vehicleReference->mDriveDynData.setUseAutoGears(true);

	//Return the reference to the car that was created
	return vehicleReference;
}

bool PhysXManager::ToggleVisualDebuggerConnection() const
{
	if (!m_pPhysics || !m_pPvd) return false;

	if (m_pPvd->isConnected())
	{
		//DISCONNECT
		m_pPvd->disconnect();
		return false;
	}

	//CONNECT
	PxPvdTransport* pTransport = m_pPvd->getTransport();
	if (!pTransport)
	{
		pTransport = PxDefaultPvdSocketTransportCreate(PVD_HOST, PVD_PORT, 10);
	}

	if (m_pPvd->connect(*pTransport, PxPvdInstrumentationFlag::eALL))
	{
		return true;
	}

	return false;
}