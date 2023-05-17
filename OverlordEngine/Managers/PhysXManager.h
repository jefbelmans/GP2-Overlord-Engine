#pragma once
class PhysxAllocator;
class PhysxErrorCallback;
class GameScene;
class OverlordGame;

#include "PhysX/Vehicle/PhysXVehicleCreate.hpp"
#include "PhysX/Vehicle/PhysXVehicleFilterShader.hpp"
#include "PhysX/Vehicle/PhysXVehicleSceneQuery.hpp"
#include "PhysX/Vehicle/PhysXVehicleTireFriction.hpp"

class PhysXManager final : public Singleton<PhysXManager>
{

public:
	PhysXManager(const PhysXManager& other) = delete;
	PhysXManager(PhysXManager&& other) noexcept = delete;
	PhysXManager& operator=(const PhysXManager& other) = delete;
	PhysXManager& operator=(PhysXManager&& other) noexcept = delete;

	PxPhysics* GetPhysics() const { return m_pPhysics; }
	PxScene* CreateScene(GameScene* pScene);

	bool ToggleVisualDebuggerConnection() const;
	bool IsPvdConnected() const { return m_pPvd && m_pPvd->isConnected(); }

	// VEHICLE
	PxVehicleDrive4W* InitializeVehicleSDK(PxRigidActor* pVehActor);

	// VEHICLE GETTERS
	PxBatchQuery* GetBatchQuery() const { return m_pBatchQuery; }
	vehicle::VehicleSceneQueryData* GetVehicleSceneQueryData() const { return m_pVehicleSceneQueryData; }
	vehicle::PxVehicleDrivableSurfaceToTireFrictionPairs* GetFrictionPairs() const { return m_pFrictionPairs; }

protected:
	void Initialize() override;

private:
	friend class Singleton<PhysXManager>;
	PhysXManager() = default;
	~PhysXManager();

	PhysxAllocator* m_pDefaultAllocator{};
	PhysxErrorCallback* m_pDefaultErrorCallback{};
	PxFoundation* m_pFoundation{};
	PxPhysics* m_pPhysics{};
	PxPvd* m_pPvd{};
	PxDefaultCpuDispatcher* m_pDefaultCpuDispatcher{};
	PxCudaContextManager* m_pCudaContextManager{};

	// VEHICLE SDK
	const int m_NrOfVehicles{ 1 };
	PxMaterial* m_pDefaultMaterial{ nullptr };
	vehicle::VehicleSceneQueryData* m_pVehicleSceneQueryData{nullptr};
	PxBatchQuery* m_pBatchQuery{ nullptr };
	vehicle::PxVehicleDrivableSurfaceToTireFrictionPairs* m_pFrictionPairs{ nullptr };
	PxScene* m_pVehicleScene{ nullptr };

	vehicle::ActorUserData m_ActorUserData{};
	vehicle::ShapeUserData m_ShapeUserData{};

	vehicle::VehicleDesc m_VehicleDesc;

	void InitializeVehicleDescription(const PxFilterData& chassisSimFilterData, const PxFilterData& wheelSimFilterData);

};
