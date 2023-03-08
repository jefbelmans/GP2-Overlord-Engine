#include "stdafx.h"
#include "RigidBodyTestScene.h"

#include "Prefabs/SpherePrefab.h"

void RigidBodyTestScene::Initialize()
{
	auto& pPhysX = PxGetPhysics();
	auto pBouncyMaterial = pPhysX.createMaterial(.6f, .6f, 1.f);

	// GROUND PLANE
	GameSceneExt::CreatePhysXGroundPlane(*this, pBouncyMaterial);

	// SPHERE RED (Group 0)
	auto pSphere = new SpherePrefab(1, 10, XMFLOAT4{ Colors::Red });
	AddChild(pSphere);

	// Translate sphere off the ground
	pSphere->GetTransform()->Translate(0.f, 30.f, 0.f);

	auto pRB = pSphere->AddComponent(new RigidBodyComponent());
	pRB->AddCollider(PxSphereGeometry{1.f}, *pBouncyMaterial);
	pRB->SetCollisionGroup(CollisionGroup::Group0);
	pRB->SetCollisionIgnoreGroups(CollisionGroup::Group1 | CollisionGroup::Group2);
	pRB->SetConstraint(RigidBodyConstraint::TransX | RigidBodyConstraint::TransZ, false);
	
	// SPHERE GREEN (Group 1)
	pSphere = new SpherePrefab(1, 10, XMFLOAT4{ Colors::Green });
	AddChild(pSphere);

	// Translate sphere off the ground
	pSphere->GetTransform()->Translate(0.f, 20.f, 0.f);

	pRB = pSphere->AddComponent(new RigidBodyComponent());
	pRB->AddCollider(PxSphereGeometry{ 1.f }, *pBouncyMaterial);
	pRB->SetCollisionGroup(CollisionGroup::Group1);
	
	// SPHERE BLUE (Group 2)
	pSphere = new SpherePrefab(1, 10, XMFLOAT4{ Colors::Blue });
	AddChild(pSphere);

	// Translate sphere off the ground
	pSphere->GetTransform()->Translate(0.f, 10.f, 0.f);

	pRB = pSphere->AddComponent(new RigidBodyComponent());
	pRB->AddCollider(PxSphereGeometry{ 1.f }, *pBouncyMaterial);
	pRB->SetCollisionGroup(CollisionGroup::Group2);
}
