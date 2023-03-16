#include "stdafx.h"
#include "SpikeyScene.h"
#include "Materials/SpikeyMaterial.h"

SpikeyScene::SpikeyScene()
	: GameScene(L"SpikeyScene")
{
}

void SpikeyScene::Initialize()
{
	// LOAD MATERIAL
	m_pSpikeyMaterial = MaterialManager::Get()->CreateMaterial<SpikeyMaterial>();

	// CREATE SPHERE
	m_pSpikeySphere = new GameObject();
	auto pTransform = m_pSpikeySphere->GetTransform();
	pTransform->Scale(15.f, 15.f, 15.f);
	pTransform->Translate(0.f, 10.f, -5.f);
	AddChild(m_pSpikeySphere);

	// LOAD MESH
	auto pModel = m_pSpikeySphere->AddComponent(new ModelComponent(L"Meshes/OctaSphere.ovm"));
	pModel->SetMaterial(m_pSpikeyMaterial);

	// SCENE SETTINGS
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.enableOnGUI = true;
}

void SpikeyScene::Update()
{
	m_RotationAngle += 20.f * m_SceneContext.pGameTime->GetElapsed();
	m_pSpikeySphere->GetTransform()->Rotate(0.f, m_RotationAngle, 0.f);
}

void SpikeyScene::Draw()
{
}

void SpikeyScene::OnGUI()
{
	m_pSpikeyMaterial->DrawImGui();
}
