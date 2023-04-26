#pragma once
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
	const PxU32 m_NrWheels{ 4 };
	GameObject* m_pChassis{ nullptr };

	PxVehicleWheelsSimData* m_pWheelsSimData{ nullptr };
	PxVehicleDriveSimData4W* m_pDriveSimData{ nullptr };

	void setupWheelsSimulationData
		(const PxF32 wheelMass, const PxF32 wheelMOI,
		const PxF32 wheelRadius, const PxF32 wheelWidth, const PxU32 numWheels,
		const PxVec3* wheelCenterActorOffsets, const PxVec3& chassisCMOffset,
		const PxF32 chassisMass, PxVehicleWheelsSimData* wheelsSimData);

	PxRigidDynamic* createVehicleActor
		(const PxVehicleChassisData& chassisData,
		PxMaterial** wheelMaterials, PxConvexMesh** wheelConvexMeshes,
		const PxU32 numWheels, const PxFilterData& wheelSimFilterData, PxMaterial** chassisMaterials,
		PxConvexMesh** chassisConvexMeshes, const PxU32 numChassisMeshes, const PxFilterData& chassisSimFilterData, PxPhysics& physics);

	void setupDriveSimData();
};

