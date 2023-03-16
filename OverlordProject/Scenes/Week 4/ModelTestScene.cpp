#include "stdafx.h"
#include "ModelTestScene.h"
#include "Materials/DiffuseMaterial.h"

ModelTestScene::ModelTestScene() :
	GameScene(L"ModelTestScene"){}

void ModelTestScene::Initialize()
{
	// PHYSX
	PxMaterial* pPhysMat = PxGetPhysics().createMaterial(0.6f, 0.6f, 0.1f);

	// LOAD MATERIAL
	DiffuseMaterial* pMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	pMaterial->SetDiffuseTexture(L"Textures/Chair_Dark.dds");

	// GROUND PLANE
	GameSceneExt::CreatePhysXGroundPlane(*this);

	// CREATE CHAIR
	m_pChair = new GameObject();
	AddChild(m_pChair);

	// LOAD MESH
	auto pModel = m_pChair->AddComponent(new ModelComponent(L"Meshes/Chair.ovm"));
	pModel->SetMaterial(pMaterial);

	// CHAIR PHYSICS
	const auto pConvexMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/Chair.ovpc");
	auto pRb = m_pChair->AddComponent(new RigidBodyComponent());
	pRb->AddCollider(PxConvexMeshGeometry{ pConvexMesh }, *pPhysMat);

	m_pChair->GetTransform()->Translate(0.f, 5.f, 0.f);
}

void ModelTestScene::Update()
{
}

void ModelTestScene::Draw()
{
}

void ModelTestScene::OnGUI()
{
}
