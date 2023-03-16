#include "stdafx.h"
#include "UberMaterialScene.h"
#include "Materials/UberMaterial.h"

UberMaterialScene::UberMaterialScene()
	: GameScene(L"UberMaterialScene")
{
}

void UberMaterialScene::Initialize()
{
	// LOAD MATERIAL
	m_pUberMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();

	// CREATE SPHERE
	m_pShpere = new GameObject();
	auto pTransform = m_pShpere->GetTransform();
	pTransform->Scale(20.f, 20.f, 20.f);
	pTransform->Translate(0.f, 10.f, -10.f);
	AddChild(m_pShpere);

	// LOAD MESH
	auto pModel = m_pShpere->AddComponent(new ModelComponent(L"Meshes/Sphere.ovm"));
	pModel->SetMaterial(m_pUberMaterial);

	// SCENE SETTINGS
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.enableOnGUI = true;
}

void UberMaterialScene::Update()
{
}

void UberMaterialScene::Draw()
{
}

void UberMaterialScene::OnGUI()
{
	m_pUberMaterial->DrawImGui();
}