//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright (c) 2008-2019 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

#include "stdafx.h"
#include "PhysXVehicleCreate.hpp"
#include "PhysXVehicleTireFriction.hpp"
#include "PhysXVehicleSceneQuery.hpp"

namespace vehicle
{

	using namespace physx;

	namespace FourWD
	{

		void computeWheelCenterActorOffsets4W(const PxF32 wheelFrontZ, const PxF32 wheelRearZ, const PxVec3& chassisDims, const PxF32 /*wheelWidth*/, const PxF32 wheelRadius, PxVec3* wheelCentreOffsets)
		{

			wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eREAR_LEFT]
				= PxVec3((-chassisDims.x) * .5f , wheelRadius, wheelRearZ + .05f);
			wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eREAR_RIGHT]
				= PxVec3((chassisDims.x) * .5f, wheelRadius, wheelRearZ + .05f);
			wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eFRONT_LEFT]
				= PxVec3((-chassisDims.x) * .5f, wheelRadius, wheelFrontZ + .3f);
			wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT]
				= PxVec3((chassisDims.x) * .5f, wheelRadius, wheelFrontZ + .3f);
		}

		void setupWheelsSimulationData
		(const PxF32 wheelMass, const PxF32 wheelMOI, const PxF32 wheelRadius, const PxF32 wheelWidth,
			const PxU32 numWheels, const PxVec3* wheelCenterActorOffsets,
			const PxVec3& chassisCMOffset, const PxF32 chassisMass,
			PxVehicleWheelsSimData* wheelsSimData)
		{
			//Set up the wheels.
			PxVehicleWheelData wheels[PX_MAX_NB_WHEELS];
			{
				//Set up the wheel data structures with mass, moi, radius, width.
				for (PxU32 i = 0; i < numWheels; i++)
				{
					wheels[i].mMass = wheelMass;
					wheels[i].mMOI = wheelMOI;
					wheels[i].mRadius = wheelRadius;
					wheels[i].mWidth = wheelWidth;
				}

				//Enable the handbrake for the rear wheels only.
				wheels[PxVehicleDrive4WWheelOrder::eREAR_LEFT].mMaxHandBrakeTorque = 4000.0f;
				wheels[PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mMaxHandBrakeTorque = 4000.0f;
				//Enable steering for the front wheels only.
				wheels[PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxSteer = PxPi * 0.25f;
				wheels[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxSteer = PxPi * 0.25f;
			}

			//Set up the tires.
			PxVehicleTireData tires[PX_MAX_NB_WHEELS];
			{
				//Set up the tires.
				for (PxU32 i = 0; i < numWheels; i++)
				{
					tires[i].mType = TIRE_TYPE_NORMAL;
				}
			}

			//Set up the suspensions
			PxVehicleSuspensionData suspensions[PX_MAX_NB_WHEELS];
			{
				//Compute the mass supported by each suspension spring.
				PxF32 suspSprungMasses[PX_MAX_NB_WHEELS];
				PxVehicleComputeSprungMasses
				(numWheels, wheelCenterActorOffsets,
					chassisCMOffset, chassisMass, 1, suspSprungMasses);

				//Set the suspension data.
				for (PxU32 i = 0; i < numWheels; i++)
				{
					suspensions[i].mMaxCompression = 0.2f;
					suspensions[i].mMaxDroop = 0.3f;
					suspensions[i].mSpringStrength = 45000.0f;
					suspensions[i].mSpringDamperRate = 4500.0f;
					suspensions[i].mSprungMass = suspSprungMasses[i];
				}

				//Set the camber angles.
				const PxF32 camberAngleAtRest = 0.f;
				const PxF32 camberAngleAtMaxDroop = 0.f;
				const PxF32 camberAngleAtMaxCompression = -0.f;
				for (PxU32 i = 0; i < numWheels; i += 2)
				{
					suspensions[i + 0].mCamberAtRest = camberAngleAtRest;
					suspensions[i + 1].mCamberAtRest = -camberAngleAtRest;
					suspensions[i + 0].mCamberAtMaxDroop = camberAngleAtMaxDroop;
					suspensions[i + 1].mCamberAtMaxDroop = -camberAngleAtMaxDroop;
					suspensions[i + 0].mCamberAtMaxCompression = camberAngleAtMaxCompression;
					suspensions[i + 1].mCamberAtMaxCompression = -camberAngleAtMaxCompression;
				}
			}

			//Set up the wheel geometry.
			PxVec3 suspTravelDirections[PX_MAX_NB_WHEELS];
			PxVec3 wheelCentreCMOffsets[PX_MAX_NB_WHEELS];
			PxVec3 suspForceAppCMOffsets[PX_MAX_NB_WHEELS];
			PxVec3 tireForceAppCMOffsets[PX_MAX_NB_WHEELS];
			{
				//Set the geometry data.
				for (PxU32 i = 0; i < numWheels; i++)
				{
					//Vertical suspension travel.
					suspTravelDirections[i] = PxVec3(0, -1, 0);

					//Wheel center offset is offset from rigid body center of mass.
					wheelCentreCMOffsets[i] =
						wheelCenterActorOffsets[i] - chassisCMOffset;

					//Suspension force application point 0.3 metres below 
					//rigid body center of mass.
					suspForceAppCMOffsets[i] =
						PxVec3(wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z);

					//Tire force application point 0.3 metres below 
					//rigid body center of mass.
					tireForceAppCMOffsets[i] =
						PxVec3(wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z);
				}
			}

			//Set up the filter data of the raycast that will be issued by each suspension.
			PxFilterData qryFilterData;
			setupNonDrivableSurface(qryFilterData);

			//Set the wheel, tire and suspension data.
			//Set the geometry data.
			//Set the query filter data
			for (PxU32 i = 0; i < numWheels; i++)
			{
				wheelsSimData->setWheelData(i, wheels[i]);
				wheelsSimData->setTireData(i, tires[i]);
				wheelsSimData->setSuspensionData(i, suspensions[i]);
				wheelsSimData->setSuspTravelDirection(i, suspTravelDirections[i]);
				wheelsSimData->setWheelCentreOffset(i, wheelCentreCMOffsets[i]);
				wheelsSimData->setSuspForceAppPointOffset(i, suspForceAppCMOffsets[i]);
				wheelsSimData->setTireForceAppPointOffset(i, tireForceAppCMOffsets[i]);
				wheelsSimData->setSceneQueryFilterData(i, qryFilterData);
				wheelsSimData->setWheelShapeMapping(i, PxI32(i));
			}

			//Add a front and rear anti-roll bar
			PxVehicleAntiRollBarData barFront;
			barFront.mWheel0 = PxVehicleDrive4WWheelOrder::eFRONT_LEFT;
			barFront.mWheel1 = PxVehicleDrive4WWheelOrder::eFRONT_RIGHT;
			barFront.mStiffness = 25000.0f;
			wheelsSimData->addAntiRollBarData(barFront);
			PxVehicleAntiRollBarData barRear;
			barRear.mWheel0 = PxVehicleDrive4WWheelOrder::eREAR_LEFT;
			barRear.mWheel1 = PxVehicleDrive4WWheelOrder::eREAR_RIGHT;
			barRear.mStiffness = 25000.0f;
			wheelsSimData->addAntiRollBarData(barRear);
		}

	} //namespace fourwheel

	PxVehicleDrive4W* createVehicle4W(const VehicleDesc& vehicle4WDesc, PxPhysics* physics, PxConvexMesh* pWheelMesh, PxConvexMesh* pChassisMesh)
	{
		const PxVec3 chassisDims = vehicle4WDesc.chassisDims;
		const PxF32 wheelWidth = vehicle4WDesc.wheelWidth;
		const PxF32 wheelRadius = vehicle4WDesc.wheelRadius;
		const PxU32 numWheels = vehicle4WDesc.numWheels;

		const PxFilterData& chassisSimFilterData = vehicle4WDesc.chassisSimFilterData;
		const PxFilterData& wheelSimFilterData = vehicle4WDesc.wheelSimFilterData;

		//Construct a physx actor with shapes for the chassis and wheels.
		//Set the rigid body mass, moment of inertia, and center of mass offset.
		PxRigidDynamic* veh4WActor = NULL;
		{
			//Assume all wheels are identical for simplicity.
			PxConvexMesh* wheelConvexMeshes[PX_MAX_NB_WHEELS];
			PxMaterial* wheelMaterials[PX_MAX_NB_WHEELS];

			//Set the meshes and materials for the driven wheels.
			for (PxU32 i = PxVehicleDrive4WWheelOrder::eFRONT_LEFT; i <= PxVehicleDrive4WWheelOrder::eREAR_RIGHT; i++)
			{
				wheelConvexMeshes[i] = pWheelMesh;
				wheelMaterials[i] = vehicle4WDesc.wheelMaterial;
			}
			//Set the meshes and materials for the non-driven wheels
			for (PxU32 i = PxVehicleDrive4WWheelOrder::eREAR_RIGHT + 1; i < numWheels; i++)
			{
				wheelConvexMeshes[i] = pWheelMesh;
				wheelMaterials[i] = vehicle4WDesc.wheelMaterial;
			}

			//Chassis just has a single convex shape for simplicity.
			PxConvexMesh* chassisConvexMeshes[1] = { pChassisMesh };
			PxMaterial* chassisMaterials[1] = { vehicle4WDesc.chassisMaterial };

			//Rigid body data.
			PxVehicleChassisData rigidBodyData;
			rigidBodyData.mMOI = vehicle4WDesc.chassisMOI;
			rigidBodyData.mMass = vehicle4WDesc.chassisMass;
			rigidBodyData.mCMOffset = vehicle4WDesc.chassisCMOffset;

			veh4WActor = createVehicleActor
			(rigidBodyData,
				wheelMaterials, wheelConvexMeshes, numWheels, wheelSimFilterData,
				chassisMaterials, chassisConvexMeshes, 1, chassisSimFilterData,
				*physics);
		}

		//Set up the sim data for the wheels.
		PxVehicleWheelsSimData* wheelsSimData = PxVehicleWheelsSimData::allocate(numWheels);
		{
			//Compute the wheel center offsets from the origin.
			PxVec3 wheelCenterActorOffsets[PX_MAX_NB_WHEELS];
			const PxF32 frontZ = chassisDims.z * 0.3f;
			const PxF32 rearZ = -chassisDims.z * 0.3f;
			FourWD::computeWheelCenterActorOffsets4W(frontZ, rearZ, chassisDims, wheelWidth, wheelRadius, wheelCenterActorOffsets);

			//Set up the simulation data for all wheels.
			FourWD::setupWheelsSimulationData
			(vehicle4WDesc.wheelMass, vehicle4WDesc.wheelMOI, wheelRadius, wheelWidth,
				numWheels, wheelCenterActorOffsets,
				vehicle4WDesc.chassisCMOffset, vehicle4WDesc.chassisMass,
				wheelsSimData);
		}

		//Set up the sim data for the vehicle drive model.
		PxVehicleDriveSimData4W driveSimData;
		{
			//Diff
			PxVehicleDifferential4WData diff;
			diff.mType = PxVehicleDifferential4WData::eDIFF_TYPE_LS_4WD;
			driveSimData.setDiffData(diff);

			//Engine
			PxVehicleEngineData engine;
			engine.mPeakTorque = 1000.0f;
			engine.mMaxOmega = 1000.0f;//approx 6000 rpm
			engine.mDampingRateFullThrottle = 0.1f;
			driveSimData.setEngineData(engine);

			//Gears
			PxVehicleGearsData gears;
			gears.mSwitchTime = 0.0f;
			driveSimData.setGearsData(gears);

			//Clutch
			PxVehicleClutchData clutch;
			clutch.mStrength = 200.0f;
			driveSimData.setClutchData(clutch);

			//Ackermann steer accuracy
			PxVehicleAckermannGeometryData ackermann;
			ackermann.mAccuracy = 1.0f;
			ackermann.mAxleSeparation =
				wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eFRONT_LEFT).z -
				wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eREAR_LEFT).z;
			ackermann.mFrontWidth =
				wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eFRONT_RIGHT).x -
				wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eFRONT_LEFT).x;
			ackermann.mRearWidth =
				wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eREAR_RIGHT).x -
				wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eREAR_LEFT).x;
			driveSimData.setAckermannGeometryData(ackermann);
		}

		//Create a vehicle from the wheels and drive sim data.
		PxVehicleDrive4W* vehDrive4W = PxVehicleDrive4W::allocate(numWheels);
		vehDrive4W->setup(physics, veh4WActor, *wheelsSimData, driveSimData, numWheels - 4);

		//Configure the userdata
		configureUserData(vehDrive4W, vehicle4WDesc.actorUserData, vehicle4WDesc.shapeUserDatas);

		//Free the sim data because we don't need that any more.
		wheelsSimData->free();

		return vehDrive4W;
	}

} //namespace vehicle


